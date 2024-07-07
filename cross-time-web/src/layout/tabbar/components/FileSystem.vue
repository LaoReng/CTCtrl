<!-- eslint-disable @typescript-eslint/no-unused-vars -->
<template>
  <!-- 树形结构 -->

  <el-tree
    default-expand-all
    style="max-width: 600px"
    empty-text="被控端无可浏览文件"
    :data="dataSource"
    node-key="id"
    highlight-current
    @node-click="clickNode"
  >
    <template #default="scope">
      <div style="width: 100%; display: flex; justify-content: space-between">
        <div style="display: flex; justify-content: left">
          <div
            style="
              display: flex;
              flex-direction: column;
              justify-content: center;
            "
            v-if="scope.node.data.type === 'folder'"
          >
            <!-- 文件夹展示 -->
            <el-icon><Folder /></el-icon>
          </div>
          <div v-else>
            <!-- 文件展示 -->
            <el-icon><Document /></el-icon>
          </div>
          <div style="margin-left: 3px">{{ scope.node.label }}</div>
        </div>
        <div style="margin-right: 20px">
          <el-popover placement="right" trigger="hover">
            <template #reference>
              <el-icon><Tools /></el-icon>
            </template>
            <div class="but-list" style="display: flex; flex-direction: column">
              <div v-if="scope.node.data.type === 'folder'">
                <el-upload
                  ref="uploadRef"
                  multiple
                  :auto-upload="false"
                  :show-file-list="false"
                  v-model:file-list="uploadFileList"
                  :on-change="uploadFileChange"
                >
                  <template #trigger>
                    <el-button
                      style="width: 124px"
                      @click="openUploadFile($event, scope.node)"
                    >
                      上传文件
                    </el-button>
                  </template>
                </el-upload>
              </div>

              <div v-if="scope.node.data.type === 'folder'" style="width: 100%">
                <el-button
                  style="width: 100%"
                  @click="openInsertFile($event, scope.node)"
                >
                  新建文件
                </el-button>
              </div>
              <div v-if="scope.node.data.type === 'folder'" style="width: 100%">
                <el-button
                  style="width: 100%"
                  @click="openInsertFolder($event, scope.node)"
                >
                  新建文件夹
                </el-button>
              </div>
              <div style="width: 100%" v-if="scope.node.data.type === 'file'">
                <el-button
                  style="width: 100%"
                  @click="downLoadEvent($event, scope.node)"
                >
                  下载
                </el-button>
              </div>
              <div style="width: 100%">
                <el-button
                  style="width: 100%"
                  @click="openUpdate($event, scope.node)"
                >
                  重命名
                </el-button>
              </div>
              <div style="width: 100%" v-if="scope.node.data.type === 'file'">
                <el-button
                  style="width: 100%"
                  @click="openDelete($event, scope.node)"
                >
                  删除
                </el-button>
              </div>
            </div>
          </el-popover>
        </div>
      </div>
    </template>
  </el-tree>

  <!-- 新增和修改文件或文件夹名称使用 -->
  <el-dialog
    v-model="fileDialogVisible"
    :title="dialogTitle"
    width="500"
    :before-close="handleClose"
    draggable
    class="rounded-dialog"
  >
    当前文件路径: {{ dialogPath }}
    <div style="display: flex; justify-content: center; margin-top: 10px">
      <el-input v-model="dialogData" placeholder="Please input" />
    </div>
    <template #footer>
      <div class="dialog-footer">
        <el-button @click="cancel">取消</el-button>
        <el-button type="primary" @click="confirm">确定</el-button>
      </div>
    </template>
  </el-dialog>

  <el-dialog
    v-model="deleteDialogVisible"
    title="删除"
    width="500"
    draggable
    class="rounded-dialog"
  >
    你确定要删除该文件吗？
    <template #footer>
      <div class="dialog-footer">
        <el-button @click="cancel">取消</el-button>
        <el-button type="primary" @click="deleteFileOrFolder">确定</el-button>
      </div>
    </template>
  </el-dialog>
</template>

<script lang="ts" setup>
import { ref, watch } from "vue";
import HomeStore from "@/store/modules/home/home";
import { ElMessage } from "element-plus";
import type Node from "element-plus/es/components/tree/src/model/node";
import { GET_TOKEN } from "@/utils/token";
const homestore = HomeStore();
const dataSource: any = ref([]);
let isInit = ref(false);
let ws: any = null;
let wsData = import.meta.env.VITE_APP_WS_URL;
watch(
  () => homestore.isShowFileSystemDialog,
  (newValue) => {
    if (newValue) {
      if (ws == null) {
        ws = new WebSocket(wsData);
      }
      ws.addEventListener("open", () => {
        let obj: any = {
          auth: GET_TOKEN(),
          opType: "0",
        };
        //上线
        ws.send(JSON.stringify(obj));
        console.log("%c客户端连接上服务端了", "color:green");
        ws.send(
          JSON.stringify({
            auth: GET_TOKEN(),
            opType: "1", //文件浏览
            EName: homestore.rightClickPreviewDeviceName,
            dir: "",
          }),
        );
      });
      ws.addEventListener("message", (data: any) => {
        if (isInit.value == false) {
          let initData = JSON.parse(data.data);
          for (let i = 0; i < initData.list.length; i++) {
            dataSource.value.push({
              label: initData.list[i].label,
              type: initData.list[i].type == 2 ? "folder" : "file",
              path: initData.list[i].label + ":/",
              //如果是folder就添加isSpread:false,否则不添加
              ...(initData.list[i].type == 2 && { isSpread: false }),
              children: [],
            });
          }
          //已经初始化，不必再进行初始化，以后消息也不会赋值
          isInit.value = true;
        }
      });
    } else {
      if (ws) {
        console.log("%c关闭连接", "color:red");

        ws.close();
        ws = null;
      }
    }
  },
  { immediate: true },
);
const fileDialogVisible = ref(false);
const dialogTitle = ref(""); // 新增文件|文件夹,修改
const dialogData = ref("");
const dialogPath = ref("/");
const deleteDialogVisible = ref(false);
const fileOrFolderNode = ref();
let tempDeleteNode = ref();
let isFlag = ref(false);

//点击文件夹
const clickNode = (node: any) => {
  //如果不是文件夹return
  if (node.type == "file") {
    return;
  }
  node.isSpread = !node.isSpread;
  //初始化为不展示，展开时发送请求，收起不让发送请求
  if (node.isSpread == false) return;
  //! 发送请求
  //如果展开，我们就清除当前children，获取新的children
  if (node.isSpread) {
    node.children = [];
    //TODO这里发送请求获取初始化的文件夹
    ws.send(
      JSON.stringify({
        auth: GET_TOKEN(),
        opType: "1",
        EName: homestore.rightClickPreviewDeviceName,
        dir: node.path,
      }),
    );
    ws.addEventListener("message", (data: any) => {
      // console.log("运行 ", data);
      // if (JSON.parse(data.data).opType == 7) {
      //   return;
      // }
      let childData = null;
      try {
        childData = JSON.parse(data.data);
      } catch (error) {
        console.log(error);
        isFlag.value = true;
      }
      if (isFlag.value) return;
      console.log(data);
      //没有权限
      let privilege = ref(false); //加锁，防止弹出多次
      if (childData.list == null) {
        if (!privilege.value) {
          ElMessage({
            type: "error",
            message: "当前无权限",
          });
        }
        privilege.value = true;
        return;
      }
      childData.list.forEach((item: any) => {
        node.children.push({
          //文件名
          label: item.label,
          type: item.type == 2 ? "folder" : "file",
          //当前的path是item.label加上我点击上一级节点的路径
          path: node.path + "/" + item.label + (item.type == 2 ? "/" : ""),
          ...(item.type == 2 && { isSpread: false }),
          children: [],
        });
      });
    });
  }
};
const handleClose = (done: () => void) => {
  cancel();
  done();
};

/**
 * 取消
 */
const cancel = () => {
  fileDialogVisible.value = false;
  deleteDialogVisible.value = false;
};

/**
 * 确定
 */
const confirm = () => {
  console.log("confirm: ", fileOrFolderNode);
  if (dialogTitle.value === "新建文件" || dialogTitle.value === "新建文件夹") {
    let data = {
      label: dialogData.value,
      type: "",
      children: [],
      path: "/" + dialogData.value,
    };

    if (fileOrFolderNode.value) {
      data.path = fileOrFolderNode.value.data.path + "/" + dialogData.value;
    }
    if (dialogTitle.value === "新建文件") {
      data.type = "file";
    } else {
      data.type = "folder";
    }
    append(fileOrFolderNode.value, data);
    //TODO 在这里传递文件的各种信息
    console.log(data);
  } else {
    // 修改处理
    let parent = null;
    let data = {
      label: dialogData.value,
      type: fileOrFolderNode.value.data.type,
      children: fileOrFolderNode.value.data.children,
      path: "/" + dialogData.value,
    };
    if (fileOrFolderNode.value.parent.level != 0) {
      parent = fileOrFolderNode.value.parent;
      data.path = parent.data.path + "/" + data.label;
    }
    updateTreeNode(parent, fileOrFolderNode.value.data, data);
    console.log(data);
  }
  fileDialogVisible.value = false;
};
//新增文件
const openInsertFile = (_even: any, node: any) => {
  if (node) {
    dialogPath.value = node.data.path + "/";
  } else {
    dialogPath.value = "/";
  }
  //TODO 新建文件获取路径
  console.log(dialogPath.value);
  dialogTitle.value = "新建文件";
  dialogData.value = "";
  fileOrFolderNode.value = node;
  fileDialogVisible.value = true;
};

//下载文件触发
let binaryData: any = ref([]);
const downLoadEvent = (_event: any, node: any) => {
  let params = {
    auth: GET_TOKEN(),
    opType: 7,
    EName: homestore.rightClickPreviewDeviceName,
    dir: node.data.path,
  };
  ws.send(JSON.stringify(params));

  ws.addEventListener("message", (event: any) => {
    //1.
    // let binaryData = event.data;
    // // 创建Blob并保存为文件
    // const blob = new Blob([binaryData], {
    //   type: "application/octet-stream",
    // });
    // console.log(blob);
    // saveAs(blob, node.data.label); // 使用fileName属性或默认文件名
    //2.
    //   const url = window.URL.createObjectURL(
    //     new Blob([event.data], {
    //       type: "image/x-icon",
    //     }),
    //   );
    //   const link = document.createElement("a");
    //   link.style.display = "none";
    //   link.href = url;
    //   link.setAttribute("download", node.data.label);
    //   document.body.appendChild(link);
    //   link.click();
    //   document.body.removeChild(link);
    //3.
    try {
      if (JSON.parse(event.data).status == "end") {
        console.log(JSON.parse(event.data));
        const blob = new Blob(binaryData.value, {
          type: "application/octet-stream",
        });

        // 创建下载链接
        const url = URL.createObjectURL(blob);

        // 创建一个 <a> 元素用于下载
        const a = document.createElement("a");
        a.style.display = "none";
        a.href = url;
        a.download = node.data.label; // 文件名可以从后端获取或者自定义
        document.body.appendChild(a);

        // 模拟点击下载链接
        a.click();

        // 清理创建的对象 URL
        URL.revokeObjectURL(url);

        // 移除 <a> 元素
        document.body.removeChild(a);
        binaryData.value = [];
        return;
      }
    } catch (error) {
      console.log(error);
    }
    binaryData.value.push(event.data);
  });
};
/**
 * 开始修改
 * @param _even
 * @param node
 */
//重命名
const openUpdate = (_even: any, node: any) => {
  if (node) {
    dialogPath.value = node.data.path;
  } else {
    dialogPath.value = "/";
  }
  fileDialogVisible.value = true;
  fileOrFolderNode.value = node;
  dialogData.value = fileOrFolderNode.value.data.label;
  dialogTitle.value = "重命名";
};

//新增文件夹
const openInsertFolder = (_even: any, node: any) => {
  if (node) {
    dialogPath.value = node.data.path;
    console.log("dialogPath: ", dialogData);
  } else {
    dialogPath.value = "/";
  }
  //TODO 新建文件夹获取路径
  console.log(dialogPath.value);
  dialogData.value = "";
  fileOrFolderNode.value = node;
  dialogTitle.value = "新建文件夹";
  fileDialogVisible.value = true;
};

const openDelete = (_even: any, node: any) => {
  fileOrFolderNode.value = node;
  tempDeleteNode.value = node;
  deleteDialogVisible.value = true;
};

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const deleteFileOrFolder = (_even: any) => {
  //TODO 删除该文件的路径
  console.log(tempDeleteNode.value.data);
  remove(fileOrFolderNode.value, fileOrFolderNode.value.data);
  deleteDialogVisible.value = false;
};

/**
 * 添加
 * @param node 父节点
 * @param data 要添加的数据
 */
const append = (node: any, data: any) => {
  if (isNameDuplicate(node, null, data)) {
    ElMessage.error("文件名重复");
    return;
  }
  const newChild = data;
  if (node) {
    if (!node.data.children) {
      node.data.children = [];
    }
    node.data.children.push(newChild);
  } else {
    dataSource.value.push(newChild);
  }
};

/**
 * 删除
 * @param node 节点
 * @param data 数据
 */
const remove = (node: Node, data: any) => {
  // console.log("all data:", dataSource.value);
  const parent = node.parent;
  const children = parent.data.children || parent.data;
  const index = children.findIndex((d: any) => d.path === data.path);
  children.splice(index, 1);
  dataSource.value = [...dataSource.value];
};

const updateTreeNode = (parentNode: any, oldData: any, newData: any) => {
  if (isNameDuplicate(parentNode, oldData, newData)) {
    ElMessage.error("文件名重复");
    return;
  }
  let index: number;
  if (parentNode && parentNode.data) {
    // 查找 newData.path 在 parentNode.data.children 中的索引
    index = parentNode.data.children.findIndex(
      (child: any) => child.path === oldData.path,
    );
    // 如果找到索引，则更新该位置的数据
    if (index !== -1) {
      parentNode.data.children[index] = newData;
    } else {
      console.error(
        "找不到, index:",
        index,
        "parentNode.data",
        parentNode.data,
        "newData",
        newData,
      );
    }
  } else {
    index = dataSource.value.findIndex(
      (item: any) => item.path === oldData.path,
    );
    // 如果找到索引，则更新该位置的数据
    if (index !== -1) {
      dataSource.value[index] = newData;
    } else {
      console.error(
        "找不到, index:",
        index,
        "parentNode.data",
        parentNode.data,
        "newData",
        newData,
      );
    }
  }

  console.log(dataSource.value);
};

/**
 * 判断名称是否有相同的
 */
const isNameDuplicate = (parentNode: any, oldData: any, newData: any) => {
  // console.log("父节点:", parentNode, "旧数据:", oldData, "新数据: ", newData);
  // console.log("新数据: ", newData);
  // console.log(newData.files[0]);
  if (oldData && oldData.label === newData.label) return false;
  if (parentNode) {
    for (let i = 0; i < parentNode.data.children.length; i++) {
      const child = parentNode.data.children[i];
      if (child.label === newData.label) {
        return true;
      }
    }
  } else {
    for (let i = 0; i < dataSource.value.length; i++) {
      const child = dataSource.value[i];
      if (child.label === newData.label) {
        return true;
      }
    }
  }

  return false;
};

/**
 * 文件上传操作
 */
const uploadFileList = ref([]);
const openUploadFile = (_even: any, node: any) => {
  if (node) {
    dialogPath.value = node.data.path;
  } else {
    dialogPath.value = "/";
  }
  fileOrFolderNode.value = node;
};

let i = 0;
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const uploadFileChange = (uploadFile: any, _uploadFiles: any) => {
  //TODO 文件上传逻辑（分片）
  // console.log("%c uploadFile:", uploadFile);
  // console.log("uploadFiles:", _uploadFiles);
  let data = {
    label: uploadFile.name,
    type: "file",
    path: "/" + uploadFile.name,
    uid: uploadFile.uid,
  };
  if (fileOrFolderNode.value) {
    data.path = fileOrFolderNode.value.data.path + "/" + uploadFile.name;
  }
  console.log(data);

  append(fileOrFolderNode.value, data);
  //上传显示列表

  homestore.downLoadList.push({
    name: data.label,
    uid: uploadFile.uid,
    percentage: 100,
    isShow: true,
  });
  homestore.isShowDownLoadList = true;
  let timer: any = null;
  //如果进度达到100%，就清除定时器
  timer = setInterval(() => {
    homestore.downLoadList[i].percentage += 10;
    if (homestore.downLoadList[i].percentage >= 100) {
      clearInterval(timer);
    }
  }, 10);
  i++;
};
</script>

<style>
.rounded-dialog {
  border-radius: 10px;
}

.but-list > div {
  margin: 2px;
}
</style>
