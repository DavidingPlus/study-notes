# 添加序列化与SPI机制

# 添加序列化方式

在设计思路那里我们有聊过各种序列化方式的优劣，本次我们添加三种序列化方式并进行测试，默认使用 Hessian.

首先来导包

```java
        <dependency>
            <groupId>com.esotericsoftware</groupId>
            <artifactId>kryo</artifactId>
            <version>4.0.2</version>
        </dependency>
        <dependency>
            <groupId>com.caucho</groupId>
            <artifactId>hessian</artifactId>
            <version>4.0.66</version>
        </dependency>
        <dependency>
            <groupId>io.protostuff</groupId>
            <artifactId>protostuff-core</artifactId>
            <version>1.7.4</version>
        </dependency>
        <dependency>
            <groupId>io.protostuff</groupId>
            <artifactId>protostuff-runtime</artifactId>
            <version>1.7.4</version>
        </dependency>
```

都放在 serializer 包下

# Hessian

Hessian将数据转换为字节流，这通常比基于文本（如 JSON、XML）的格式更加高效。在网络传输和存储方面，二进制格式相比于文本格式占用更少的带宽和存储空间。

dubbo 的默认序列化方式就是 Hessian

```java
public class HessianSerializer implements Serializer {
    @Override
    public byte[] serialize(Object obj) {
        // 使用 ByteArrayOutputStream 和 HessianOutput 来实现对象的序列化
        try (ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream()) {
            HessianOutput hessianOutput = new HessianOutput(byteArrayOutputStream);
            hessianOutput.writeObject(obj);  // 将对象写入输出流
            return byteArrayOutputStream.toByteArray();  // 返回字节数组
        } catch (IOException e) {
            throw new SerializeException("Serialization failed");
        }
    }

    @Override
    public Object deserialize(byte[] bytes, int messageType) {
        // 使用 ByteArrayInputStream 和 HessianInput 来实现反序列化
        try (ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(bytes)) {
            HessianInput hessianInput = new HessianInput(byteArrayInputStream);
            return hessianInput.readObject();  // 读取并返回对象
        } catch (IOException e) {
            throw new SerializeException("Deserialization failed");
        }
    }

    @Override
    public int getType() {
        return 3;
    }

    @Override
    public String toString() {
        return "Hessian";
    }
}
```

## 测试类

```java
public class HessianSerializerTest {

    private HessianSerializer serializer = new HessianSerializer();

    @Test
    public void testSerializeAndDeserialize() {
        // 创建一个测试对象
        String original = "Hello, Hessian!";

        // 序列化
        byte[] serialized = serializer.serialize(original);
        assertNotNull("序列化结果不应为 null", serialized);

        // 反序列化
        Object deserialized = serializer.deserialize(serialized, 3);
        assertNotNull("反序列化结果不应为 null", deserialized);

        // 校验反序列化的结果
        assertEquals("反序列化的对象应该与原对象相同", original, deserialized);
    }

    @Test
    public void testDeserializeWithInvalidData() {
        byte[] invalidData = new byte[]{1, 2, 3}; // 假数据

        // 测试无效数据反序列化
        try {
            serializer.deserialize(invalidData, 3);
            fail("反序列化时应抛出异常");
        } catch (SerializeException e) {
            assertEquals("Deserialization failed", e.getMessage());
        }
    }
}
```

# Kryo

Kryo 的设计目标是提供更高效、更快速的序列化比 Java 自带的序列化机制（如 `Serializable` 接口）要高效得多。Kryo 主要用于处理 Java 对象的二进制序列化，常常被用在高性能、低延迟的应用中。

```java
public class KryoSerializer implements Serializer {
    private Kryo kryo;

    public KryoSerializer() {
        this.kryo = new Kryo();
    }

    @Override
    public byte[] serialize(Object obj) {
        if (obj == null) {
            throw new IllegalArgumentException("Cannot serialize null object");
        }

        try (ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
             Output output = new Output(byteArrayOutputStream)) {

            kryo.writeObject(output, obj); // 使用 Kryo 写入对象
            return output.toBytes(); // 返回字节数组

        } catch (Exception e) {
            throw new SerializeException("Serialization failed");
        }
    }

    @Override
    public Object deserialize(byte[] bytes, int messageType) {
        if (bytes == null || bytes.length == 0) {
            throw new IllegalArgumentException("Cannot deserialize null or empty byte array");
        }

        try (ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(bytes);
             Input input = new Input(byteArrayInputStream)) {

            // 根据 messageType 来反序列化不同的类
            Class<?> clazz = getClassForMessageType(messageType);
            return kryo.readObject(input, clazz); // 使用 Kryo 反序列化对象

        } catch (Exception e) {
            throw new SerializeException("Deserialization failed");
        }
    }

    @Override
    public int getType() {
        return 2;
    }

    private Class<?> getClassForMessageType(int messageType) {
        if (messageType == 1) {
            return User.class;  // 假设我们在此反序列化成 User 类
        } else {
            throw new SerializeException("Unknown message type: " + messageType);
        }
    }

    @Override
    public String toString() {
        return "Kryo";
    }
}
```

## 测试类

```java
public class KryoSerializerTest {

    private KryoSerializer serializer = new KryoSerializer();

    @Test
    public void testSerializeAndDeserialize() {
        // 创建一个 User 对象
        User originalUser = User.builder()
        .id(1)
        .userName("TestUser")
        .gender(true)
        .build();

        // 序列化
        byte[] serialized = serializer.serialize(originalUser);
        assertNotNull("序列化结果不应为 null", serialized);

        // 反序列化
        Object deserialized = serializer.deserialize(serialized, 1);
        assertNotNull("反序列化结果不应为 null", deserialized);

        // 校验反序列化的对象是否与原对象相同
        assertTrue("反序列化的对象应该是 User 类型", deserialized instanceof User);
        User deserializedUser = (User) deserialized;
        assertEquals("反序列化的 User 应该与原 User 相同", originalUser, deserializedUser);
    }

    @Test
    public void testSerializeNullObject() {
        // 测试序列化 null 对象
        try {
            serializer.serialize(null);
            fail("序列化 null 对象时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot serialize null object", e.getMessage());
        }
    }

    @Test
    public void testDeserializeNullBytes() {
        // 测试反序列化 null 或空字节数组
        try {
            serializer.deserialize(null, 1);
            fail("反序列化 null 字节数组时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot deserialize null or empty byte array", e.getMessage());
        }
    }

    @Test
    public void testDeserializeEmptyBytes() {
        // 测试反序列化空字节数组
        try {
            serializer.deserialize(new byte[0], 1);
            fail("反序列化空字节数组时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot deserialize null or empty byte array", e.getMessage());
        }
    }

    @Test
    public void testDeserializeInvalidMessageType() {
        // 测试反序列化未知的 messageType
        byte[] serialized = serializer.serialize(new User(1, "TestUser", true));
        try {
            serializer.deserialize(serialized, 99); // 使用无效的 messageType
            fail("反序列化时应抛出 SerializeException");
        } catch (SerializeException e) {
            assertEquals("Deserialization failed", e.getMessage());
        }
    }
}
```

# Protostuff

设计目标是提供比 Java 原生序列化机制更高效的二进制序列化解决方案，同时也提供与其他流行的序列化框架（如 Protocol Buffers 和 Thrift）相比具有竞争力的性能。

```java
public class ProtostuffSerializer implements Serializer {

    @Override
    public byte[] serialize(Object obj) {
        // 检查 null 对象
        if (obj == null) {
            throw new IllegalArgumentException("Cannot serialize null object");
        }
        // 获取对象的 schema
        Schema schema = RuntimeSchema.getSchema(obj.getClass());

        // 使用 LinkedBuffer 来创建缓冲区（默认大小 1024）
        LinkedBuffer buffer = LinkedBuffer.allocate(LinkedBuffer.DEFAULT_BUFFER_SIZE);
        // 序列化对象为字节数组
        byte[] bytes;
        try {
            bytes = ProtostuffIOUtil.toByteArray(obj, schema, buffer);
        } finally {
            buffer.clear();
        }
        return bytes;
    }

    @Override
    public Object deserialize(byte[] bytes, int messageType) {
        if (bytes == null || bytes.length == 0) {
            throw new IllegalArgumentException("Cannot deserialize null or empty byte array");
        }

        // 根据 messageType 来决定反序列化的类，这里假设 `messageType` 是类的标识符
        Class<?> clazz = getClassForMessageType(messageType);

        // 获取对象的 schema
        Schema schema = RuntimeSchema.getSchema(clazz);

        // 创建一个空的对象实例
        Object obj;
        try {
            obj = clazz.getDeclaredConstructor().newInstance();
        } catch (Exception e) {
            throw new SerializeException("Deserialization failed due to reflection issues");
        }

        // 反序列化字节数组为对象
        ProtostuffIOUtil.mergeFrom(bytes, obj, schema);
        return obj;
    }

    @Override
    public int getType() {
        return 4;
    }

    // 用于根据 messageType 获取对应的类
    private Class<?> getClassForMessageType(int messageType) {
        if (messageType == 1) {
            return User.class;  // 假设我们在此反序列化成 User 类
        } else {
            throw new SerializeException("Unknown message type: " + messageType);
        }
    }

    @Override
    public String toString() {
        return "Protostuff";
    }
}
```

## 测试类

```java
public class ProtostuffSerializerTest {

    private ProtostuffSerializer serializer = new ProtostuffSerializer();

    @Test
    public void testSerializeAndDeserialize() {
        // 创建一个 User 对象
        User originalUser = User.builder()
        .id(1)
        .userName("TestUser")
        .gender(true)
        .build();

        // 序列化
        byte[] serialized = serializer.serialize(originalUser);
        assertNotNull("序列化结果不应为 null", serialized);

        // 反序列化
        Object deserialized = serializer.deserialize(serialized, 1);
        assertNotNull("反序列化结果不应为 null", deserialized);

        // 校验反序列化的对象是否与原对象相同
        assertTrue("反序列化的对象应该是 User 类型", deserialized instanceof User);
        User deserializedUser = (User) deserialized;
        assertEquals("反序列化的 User 应该与原 User 相同", originalUser, deserializedUser);
    }

    @Test
    public void testSerializeNullObject() {
        // 测试序列化 null 对象
        try {
            serializer.serialize(null);
            fail("序列化 null 对象时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot serialize null object", e.getMessage());
        }
    }

    @Test
    public void testDeserializeNullBytes() {
        // 测试反序列化 null 字节数组
        try {
            serializer.deserialize(null, 1);
            fail("反序列化 null 字节数组时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot deserialize null or empty byte array", e.getMessage());
        }
    }

    @Test
    public void testDeserializeEmptyBytes() {
        // 测试反序列化空字节数组
        try {
            serializer.deserialize(new byte[0], 1);
            fail("反序列化空字节数组时应抛出 IllegalArgumentException");
        } catch (IllegalArgumentException e) {
            assertEquals("Cannot deserialize null or empty byte array", e.getMessage());
        }
    }

    @Test
    public void testDeserializeInvalidMessageType() {
        // 测试反序列化未知的 messageType
        byte[] serialized = serializer.serialize(new User(1, "TestUser", true));
        try {
            serializer.deserialize(serialized, 99); // 使用无效的 messageType
            fail("反序列化时应抛出 SerializeException");
        } catch (SerializeException e) {
            assertEquals("Unknown message type: 99", e.getMessage());
        }
    }
}
```

好的，那么序列化方式就添加完毕了，后续可以根据配置顶选择使用的序列化方式。

# 什么是 SPI？

**SPI（Service Provider Interface）机制** 是 Java 提供的一种服务发现和扩展机制，它允许开发者在运行时根据需求动态加载和调用服务的具体实现。SPI 是一种松耦合的设计模式，广泛应用于 Java 平台中，用于插件式架构的实现，帮助系统在不修改核心代码的情况下扩展功能。

# 如何实现 SPI？

我们先在 resources 下创建一个 META-INF 包 ,在这下面创建一个 serializer 的包

创建一个文件名为你的序列化方式的路径，我的这里是 com.kama.common.serializer.myserializer

文件中放这些序列化方式。

```java
Hessian=com.kama.common.serializer.myserializer.HessianSerializer
protobuf=com.kama.common.serializer.myserializer.ProtobufSerializer
json=com.kama.common.serializer.myserializer.JsonSerializer
kryo=com.kama.common.serializer.myserializer.KryoSerializer
jdk=com.kama.common.serializer.myserializer.ObjectSerializer

```

这时我们就得到一个配置文件序列化名->对应序列化对象的映射，之后就可以根据用户配置的序列化器名称动态加载指定实现类对象。


> 更新: 2024-12-06 15:45:03  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/iw3hyhc7x4inwxoo>