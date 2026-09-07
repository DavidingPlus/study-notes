# part1 netty自定义编码器，解码器和序列化器

# netty中的重要组件：

ChannelHandler 与 ChannelPipeline ChannelHandler 是对 Channel 中数据的处理器，这些处理器可以是系统本身定义好的编解码器，也可以是用户自定义的。这些处理器会被统一添加到一个 ChannelPipeline 的对象中，然后按照添加的类别对 Channel 中的数据进行依次处理。

在上一节中，我们使用netty自带的编码器和解码器 来实现数据的传输

而在这里，我们可以通过继承netty提供的基类，实现**自定义的编码器和解码器**

# <font style="color:rgb(55, 65, 81);">使用自定义编/解码器的好处？</font>

* <font style="color:rgb(55, 65, 81);">将编码解码的过程进行封装，代码变得简洁易读，维护更加方便</font>
* <font style="color:rgb(55, 65, 81);">在内部实现消息头的加工，解决沾包问题</font>
* <font style="color:rgb(55, 65, 81);">消息头中加入messageType消息类型，对消息的读取机制有了进一步的拓展</font>

# 整体顺序

JSON 之前要导包

在 RpcResponse 中加入数据类型字段。

Serializer->ObjectSerializer->JsonSerializer->MessageType->MyEncoder->MyDecoder->修改nettyInitializer

# Serializer

```java
/*
*用于为对象提供序列化和反序列化的功能
*通过一个静态工厂方法根据类型代码返回具体的序列化器实例。
*/
public interface Serializer {
    //把对象序列化为字节数组
    byte[] serialize(Object obj);
    //将一个字节数组反序列化为Java对象
    //如果用Java自带的不用messageType也能得到相应的对象
    //其他方式需要指定消息格式，再根据messageType转化成消息对象
    Object deserializer(byte[] bytes, int messageType);
    //返回使用的序列化器是哪个
    //0为Java自带的序列化方式，1为json
    int getType();
    //静态工厂方法
    //根据序号取出序列化器，暂时有两种实现方式，需要其他方式，实现这个接口即可
    static Serializer getSerializerByCode(int code) {
        switch (code) {
            case 0:
                return new ObjectSerilaizer();
            case 1:
                return new JsonSerializer();
            default:
                return null;
        }
    }
}
```

## 1. 为什么要序列化为字节数组？

* 网络通信中，数据只能以字节流形式进行传输。\
  将对象序列化为字节数组后，能够通过网络传输，并在接收方反序列化为原始对象。
* 字节数组是一种通用的数据表示形式，不依赖于特定语言或平台。

## 2.这句是为了什么？

```java
 Object deserializer(byte[] bytes, int messageType);
```

反序列化，根据字节数组和消息类型得到一个消息对象

## 3.为什么设置为静态的

```plain
static Serializer getSerializerByCode(int code) {
    switch (code) {
        case 0:
            return new ObjectSerilaizer();
        case 1:
            return new JsonSerializer();
        default:
            return null;
    }
}
```

将getSerializerByCode 设置为静态的，是因为他是一个与实例无关的静态方法，仅负责逻辑判断和返回结果。

# ObjectSerializer

```java
public class ObjectSerializer implements Serializer {
    @Override
    public byte[] serialize(Object obj) {
        byte[] bytes = null;
        //创建一个内存中的输出流，用于存储序列化后的字节数据
        //ByteArrayOutputStream是一个可变大小的字节数据缓冲区，数据都会写入这个缓冲区中
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        try {
            //将对象转换为二进制数据，把对象数据写入字节缓冲区bos
            ObjectOutputStream oos = new ObjectOutputStream(bos);
            //把对象写入输出流中，触发序列化
            oos.writeObject(obj);
            //强制将缓冲区的数据刷新到底层流bos中
            oos.flush();
            //将字节缓冲区的内容转换为字节数组
            bytes = bos.toByteArray();
            oos.close();
            bos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return bytes;
    }

    @Override
    public Object deserializer(byte[] bytes, int messageType) {
        Object obj = null;
        //把字节数组包装成一个字节输入流
        ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
        try {
            //使用 ObjectInputStream 包装一个 ByteArrayInputStream 对象
            ObjectInputStream ois = new ObjectInputStream(bis);
            //从 ois 中读取序列化的对象，并将其反序列化为 Java 对象
            obj = ois.readObject();
            ois.close();
            bis.close();
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
        return obj;
    }

    @Override
    public int getType() {
        return 0;
    }
}
```

## 1.bos 与 bis 这两段代码是做什么的？

bos 是创建一个输出流，存储序列化后的字节数据。

ByteArrayOutputStream是一个可变大小的字节数据缓冲区，数据都会写入这个缓冲区中

## 2.本类如何进行序列化和反序列化的？

### 序列化过程

1.创建输出流，使用 `ByteArrayOutputStream`，在内存中创建一个动态缓冲区，用于存储序列化后的字节数据。

2.包装输出流， 使用 `ObjectOutputStream` 包装 `ByteArrayOutputStream`，将对象写入字节缓冲区。

3.序列化对象， 调用 `ObjectOutputStream.writeObject(obj)` 方法，触发对象的序列化。

4.转化为字节数组，使用 `ByteArrayOutputStream.toByteArray()` 获取缓冲区中的字节数据。

5.关闭流， 关闭 `ObjectOutputStream` 和 `ByteArrayOutputStream`。

### 反序列化过程

1.创建输入流， 使用 `ByteArrayInputStream`，将字节数组包装为字节输入流。

2.包装输入流， 使用 `ObjectInputStream` 包装 `ByteArrayInputStream`，以支持反序列化。

3.反序列化对象， 调用 `ObjectInputStream.readObject()` 方法，将字节流数据解析为 Java 对象。

4.关闭流， 关闭 `ObjectInputStream` 和 `ByteArrayInputStream`。

# JsonSerializer

```java
public class JsonSerializer implements Serializer {
    @Override
    public byte[] serialize(Object obj) {
        //将对象转化为json格式的字符的数组
        byte[] bytes = JSONObject.toJSONBytes(obj);
        return bytes;
    }

    @Override
    public Object deserializer(byte[] bytes, int messageType) {
        Object obj = null;
        switch (messageType) {
            case 0:
                //将字节数组转化为RpcRequest对象
                RpcRequest request = JSONObject.parseObject(bytes, RpcRequest.class);
                //创建一个 Object 类型的数组，用于存储解析后的请求参数
                Object[] objects = new Object[request.getParamsType().length];
                //fastjson可以读出基本数据类型，不用转化
                //对转化后的request的params属性进行类型判断
                for (int i = 0; i < objects.length; i++) {
                    //paramsType是目标参数类型，request.getParamsType()[i]是类型数组，每个元素表示参数目标类型
                    //由RPC框架在调用方法时动态决定
                    Class<?> paramsType = request.getParamsType()[i];
                    //如果类型兼容，则直接赋值，否则使用fastjson进行类型转换
                    if (!paramsType.isAssignableFrom(request.getParams()[i].getClass())) {
                        objects[i] = JSONObject.toJavaObject((JSONObject) request.getParams()[i], request.getParamsType()[i]);
                    } else {
                        objects[i] = request.getParams()[i];
                    }
                }
                //将转换后的参数列表赋值回request对象的params属性
                //这样，RpcRequest对象的params就是一个与paramsType对应的、类型正确的参数数组。
                request.setParams(objects);
                obj = request;
                break;
            case 1:
                //将字节数组转化为RpcResponse对象
                RpcResponse response = JSONObject.parseObject(bytes, RpcResponse.class);
                //根据对象获取对象类型
                Class<?> dataType = response.getDataType();
                //如果类型兼容，则直接赋值，否则使用fastjson进行类型转换
                //谁与谁的类型兼容？目标类型与实际返回数据类型
                if (!dataType.isAssignableFrom(response.getData().getClass())) {
                    response.setData(JSONObject.toJavaObject((JSONObject) response.getData(), dataType));
                }
                obj = response;
                break;
            default:
                System.out.println("暂时不支持这种消息");
                throw new RuntimeException();
        }
        return obj;
    }

    @Override
    public int getType() {
        return 1;
    }
}

```

## 1.这是 json 自带的方法吗 作用是什么？

```java
JSONObject.toJSONBytes(obj);

JSON.parseObject(bytes, RpcRequest.class);

request.getParamsType()[i]

.isAssignableFrom(request.getParams()[i].getClass())

JSONObject.toJavaObject((JSONObject) request.getParams()[i],request.getParamsType()[i]);

.getDataType()
```

* 使用 `fastjson` 提供的 `JSONObject.toJSONBytes` 方法，将任意对象转换为 JSON 格式的字节数组。
* 这句话是使用 `FastJSON` 库中的 `JSONObject.parseObject` 方法，将字节数组（`bytes`）反序列化为一个 Java 对象（这里是 `RpcRequest` 类型）。
* `request.getParamsType()[i]` 是 **请求参数的目标类型**
* <code>**.isAssignableFrom(request.getParams()[i].getClass())**</code>\*\* 是一个类型检查操作，用于判断 **<code>**request.getParams()[i]**</code>** 的实际类型 是否与目标类型 兼容。这里涉及的是 Java 反射机制中的方法 **<code>**Class.isAssignableFrom(Class<?>)**</code>**。  \*\*
* <code>**JSONObject.toJavaObject((JSONObject) request.getParams()[i], request.getParamsType()[i]);**</code>\*\* 是使用 FastJSON 的一个方法，将 **<code>**JSONObject**</code>** 类型的数据转换为指定的目标 Java 类型。  \*\*
* <code>**.getDataType()**</code>\*\* 是一个方法，用于获取某个对象的 数据类型信息。  \*\*

## 2.反序列化得到对象，如何将对象的类型进行判断？

判断目标对象和实际返回的对象是否兼容用\*\*.isAssignableFrom(request.getParams()\[i].getClass())方法判断。\*\*

# MessageType

```java
@AllArgsConstructor
public enum MessageType {
    // 枚举常量，代表消息请求
    REQUEST(0),
    // 枚举常量，代表消息响应
    RESPONSE(1);
    //每个枚举值对应的编码
    private  int code;
    //提供对code值得访问
    public int getCode() {
        return code;
    }
}
```

## 1.REQUEST 和 RESPONSE 是干什么的？

是枚举常量，定义了两个枚举值分别代表消息得请求和响应。

# MyEncoder

```java
//MessageToByteEncoder是netty专门设计用来实现编码器得抽象类，可以帮助开发者将Java对象编码成字节数据。
@AllArgsConstructor
public class MyEncoder extends MessageToByteEncoder {
    private Serializer serializer;
    //netty在写出数据时会调用这个方法，将Java对象编码成二进制数据
    //参数ctx 是netty提供得上下文对象，代表管道上下文，包含通道和处理器相关信息。
    //参数msg是要编码得消息对象
    //参数out 是netty提供的字节缓冲区，编码后的字节数据写入其中
    @Override
    protected void encode(ChannelHandlerContext ctx, Object msg, ByteBuf out) throws Exception {
        //打印消息对象的类名，用于调试编码过程中消息的类型。
        System.out.println(msg.getClass());
        //判断消息是否是RpcRequest或RpcResponse类型，根据类型写入类型标识
        if (msg instanceof RpcRequest) {
            out.writeShort(MessageType.REQUEST.getCode());
        } else if (msg instanceof RpcResponse) {
            out.writeShort(MessageType.RESPONSE.getCode());
        }
        //写入当前序列化器的类型标识（short类型）
        out.writeShort(serializer.getType());
        //将消息转化为字符数组
        byte[] serializeBytes = serializer.serialize(msg);
        //写入消息的字节长度
        out.writeInt(serializeBytes.length);
        //将字节数据内容写入输出缓冲区中
        out.writeBytes(serializeBytes);
    }
}
```

## 1.MyEncoder 为什么继承这个类？

MessageToByteEncoder是netty专门设计用来实现编码器得抽象类，可以帮助开发者将Java对象编码成字节数据。

## 2.encode 的三个参数？

参数ctx 是netty提供得上下文对象，代表管道上下文，包含通道和处理器相关信息。

参数msg是要编码得消息对象

参数out 是netty提供的字节缓冲区，编码后的字节数据写入其中

## 3. 为什么要输出这句话？

```java
System.out.println(msg.getClass());
```

打印消息对象的类名，用于调试编码过程中消息的类型。

## 4 这两个判断是做什么的？

```java
if(msg instanceof RpcRequest){
    out.writeShort(MessageType.REQUEST.getCode());
}
else if(msg instanceof RpcResponse){
    out.writeShort(MessageType.RESPONSE.getCode());
}
```

判断消息是否是RpcRequest或RpcResponse类型，根据类型写入类型标识

# MyDecoder

```java
public class MyDecoder extends ByteToMessageDecoder {
    //它负责传入的字节流解码为业务对象，并将解码后的对象添加到out中，供下一个handler处理
    //ctx是Netty的ChannelHandlerContext对象，提供对管道、通道和事件的访问
    //in是ByteBuf对象，接收到的字节流，它是netty的缓冲区，可以理解为字节数组
    //out是List对象，用于存储解码后的对象
    @Override
    protected void decode(ChannelHandlerContext ctx, ByteBuf in, List<Object> out) throws Exception {
        //读取消息类型
        short messageType = in.readShort();
        //判断是否是请求或响应消息
        if (messageType != MessageType.REQUEST.getCode() && messageType != MessageType.RESPONSE.getCode()) {
            System.out.println("暂不支持此种数据");
        }
        //读取序列化类型
        Short serializerType = in.readShort();
        //获取对应的序列化对象，根据类型返回一个适当的序列化器
        Serializer serializer = Serializer.getSerializerByCode(serializerType);
        if (serializer == null) {
            throw new RuntimeException("不存在对应的序列化器");
        }
        //读取消息长度和数据
        int length = in.readInt();
        byte[] bytes = new byte[length];
        //将消息内存序列化后存到字节数组中
        in.readBytes(bytes);
        //反序列化对象后赋值变量中
        Object deserialize = serializer.deserializer(bytes, messageType);
        //将对象添加到out列表中
        out.add(deserialize);
    }
}
```

## 1. 解释一下 decode 的参数作用？

它负责传入的字节流解码为业务对象，并将解码后的对象添加到out中，供下一个handler处理

## 2.解码器需要读取出哪些数据？

消息类型

序列化类型

消息的长度和数据

# 修改nettyInitializer

## 客户端 NettyInitializer

```java
public class NettyClientInitializer extends ChannelInitializer<SocketChannel> {
    @Override
    protected void initChannel(SocketChannel ch) throws Exception {
        ChannelPipeline pipeline = ch.pipeline();
        //使用自定义的编解码器
        pipeline.addLast(new MyDecoder());
        pipeline.addLast(new MyEncoder(new JsonSerializer()));
        pipeline.addLast(new NettyClientHandler());
    }
}
```

## 服务端NettyInitializer

```java
@AllArgsConstructor
public class NettyServerInitializer extends ChannelInitializer<SocketChannel> {
    private ServiceProvider serviceProvider;
    @Override
    protected void initChannel(SocketChannel ch) throws Exception {
        ChannelPipeline pipeline = ch.pipeline();
        //自定义编解码器
        pipeline.addLast(new MyDecoder());
        pipeline.addLast(new MyEncoder(new JsonSerializer()));
        pipeline.addLast(new NettyRPCServerHandler(serviceProvider));
    }
}
```


> 更新: 2024-11-29 19:22:19  
> 原文: <https://www.yuque.com/chengxuyuancarl/gykg94/gpsygln1rk615ly1>