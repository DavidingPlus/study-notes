# MapStruct实现Bean的快速Copy

**MapStruct** 是一个 Java 编译时期的类型安全 Bean 映射工具，主要用于简化不同对象之间的数据转换，例如 DTO ↔ Entity、VO ↔ DO 的映射，避免繁琐重复的 setter/getter 编写，提高代码可维护性与开发效率。

## 一、适用场景

* 前后端数据传输对象（DTO）与数据库实体对象（Entity/DO）的转换。
* 多层架构中业务对象（BO）和展示对象（VO）之间的数据传递。
* 字段名称不一致、类型不同等复杂场景的映射处理。

## 二、依赖引入（Maven）

在 `pom.xml` 中添加如下依赖：

```java
<dependency>
    <groupId>org.mapstruct</groupId>
    <artifactId>mapstruct</artifactId>
    <version>1.6.0.Beta1</version>
</dependency>



 <build>
     <plugins>
         <plugin>
             <groupId>org.apache.maven.plugins</groupId>
             <artifactId>maven-compiler-plugin</artifactId>
             <version>3.11.0</version>
             <configuration>
                 <annotationProcessorPaths>
                     <path>
                         <groupId>org.projectlombok</groupId>
                         <artifactId>lombok</artifactId>
                         <version>1.18.30</version>
                     </path>
                     <path>
                         <groupId>org.mapstruct</groupId>
                         <artifactId>mapstruct-processor</artifactId>
                         <version>1.5.5.Final</version>
                     </path>
                 </annotationProcessorPaths>
             </configuration>
         </plugin>
        ...
    </plugins>
</build>
```

## 三、Convert定义

```java
@Mapper(nullValueCheckStrategy = NullValueCheckStrategy.ALWAYS, componentModel = "spring")
public interface FileConvertor {


    @Mapping(target = "parentId", expression = "java(com.disk.base.utils.IdUtil.decrypt(createFolderParam.getParentId()))")
    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    CreateFolderContext createFolderParamToCreateFolderContext(CreateFolderParamVO createFolderParam);

    @Mapping(target = "fileId", expression = "java(com.disk.base.utils.IdUtil.decrypt(updateFilenameParam.getFileId()))")
    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    UpdateFilenameContext updateFilenameParamToUpdateFilenameContext(UpdateFilenameParamVO updateFilenameParam);

    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    DeleteUserFileContext deleteFileParamToDeleteFileContext(DeleteFileParamVO deleteFileParam);

    List<UserFileVO> mapToVo(List<UserFileDO> request);

    List<FileSearchVO> mapToSearchVo(List<UserFileDO> request);

    @Mapping(target = "parentId", expression = "java(com.disk.base.utils.IdUtil.decrypt(secUploadFileParam.getParentId()))")
    SecUploadFileContext secUploadFileParamToSecUploadFileContext(SecUploadFileParamVO secUploadFileParam);

    @Mapping(target = "parentId", expression = "java(com.disk.base.utils.IdUtil.decrypt(fileUploadParam.getParentId()))")
    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    UploadFileContext fileUploadParamToFileUploadContext(FileUploadParamVO fileUploadParam);

    @Mapping(target = "fileRecord", ignore = true)
    SaveFileContext fileUploadContextToFileSaveContext(UploadFileContext context);

    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    FileChunkUploadContext fileChunkUploadParamToFileChunkUploadContext(FileChunkUploadParamVO fileChunkUpload);


    FileChunkSaveContext fileChunkUploadContextToFileChunkSaveContext(FileChunkUploadContext context);

    @Mapping(target = "realPath", ignore = true)
    StoreFileChunkContext fileChunkSaveContext2StoreFileChunkContext(FileChunkSaveContext context);

    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    QueryUploadedChunksContext queryUploadedChunksPO2QueryUploadedChunksContext(QueryUploadedChunkListParamVO queryUploadedChunksPO);

    @Mapping(target = "userId", expression = "java(com.disk.base.utils.UserIdUtil.get())")
    @Mapping(target = "parentId", expression = "java(com.disk.base.utils.IdUtil.decrypt(fileChunkMergeParam.getParentId()))")
    FileChunkMergeContext fileChunkMergeParamVOToFileChunkMergeContext(FileChunkMergeParamVO fileChunkMergeParam);

    @Mapping(target = "name", source = "record.filename")
    @Mapping(target = "id", source = "record.id")
    @Mapping(target = "children", expression = "java(com.google.common.collect.Lists.newArrayList())")
    FolderTreeNodeVO userFile2FolderTreeNodeVO(UserFileDO record);

    UserFileVO userFileToUserFileVO(UserFileDO record);

    FileChunkMergeAndSaveContext fileChunkMergeContext2FileChunkMergeAndSaveContext(FileChunkMergeContext context);

    UserFileData userFileDOToUserFileData(UserFileDO context);
}
```


> 更新: 2025-08-03 18:11:11  
> 原文: <https://www.yuque.com/chengxuyuancarl/edybxb/vfy6fwgq1lrzovog>