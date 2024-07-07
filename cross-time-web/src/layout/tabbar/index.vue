<template>
  <div class="tabbar">
    <div class="tabbar_left">
      <div class="leftContent">
        <el-icon class="icon" style="margin-right: 10px" @click="changeIcon">
          <component :is="useSettingStore.fold ? 'Fold' : 'Expand'"></component>
        </el-icon>
        <!-- 左侧面包屑 -->
        <el-breadcrumb class="bread" :separator-icon="ArrowRight">
          <el-breadcrumb-item
            v-for="item in $route.matched"
            :to="item.path"
            v-show="item.meta.title"
            :key="item.path"
          >
            <div style="display: flex; align-items: center">
              <!-- 展示路由图标 -->
              <el-icon>
                <component :is="item.meta.icon"></component>
              </el-icon>
              <!-- 展示路由名字 -->
              {{ item.meta.title }}
            </div>
          </el-breadcrumb-item>
        </el-breadcrumb>
      </div>
    </div>
    <div class="tabbar_center">
      <NoticesComponent />
    </div>
    <div class="tabbar_right">
      <el-tooltip effect="dark" content="关机" placement="top-start">
        <el-button
          v-show="homestore.equipInfo.equipments"
          @click="homestore.ShutdownOrReboot(1)"
          type="danger"
          size="small"
          circle
          :icon="SwitchButton"
        ></el-button>
      </el-tooltip>
      <el-tooltip effect="dark" content="重启" placement="top-start">
        <el-button
          v-show="homestore.equipInfo.equipments"
          @click="homestore.ShutdownOrReboot(2)"
          color="#CDBE70"
          :icon="Loading"
          size="small"
          circle
        />
      </el-tooltip>
      <el-tooltip effect="dark" content="客户端下载" placement="top-start">
        <el-button
          type="primary"
          size="small"
          circle
          color="rgb(22,196,175)"
          :icon="Monitor"
          @click="toClientCenter"
        ></el-button>
      </el-tooltip>

      <el-tooltip effect="dark" content="刷新" placement="top-start">
        <el-button
          @click="refreshEvent"
          type="primary"
          :icon="RefreshRight"
          size="small"
          circle
        />
      </el-tooltip>

      <!-- 上传 -->
      <el-popover
        placement="bottom"
        :width="200"
        trigger="hover"
        style="z-index: 999"
      >
        <template #default>
          <div
            v-if="homestore.downLoadList.length == 0"
            style="text-align: center"
          >
            暂无上传列表
          </div>
          <div v-else>
            <div style="text-align: center">上传列表</div>
            <div
              class="list"
              v-for="(item, index) in homestore.downLoadList"
              :key="item.uid"
            >
              <!-- 上传文件名 -->
              <div
                style='
                font-family: "Franklin Gothic Medium&quot,
                  "Arial Narrow", Arial, sans-serif;
              '
              >
                {{ item.name }}
              </div>
              <!-- 进度条 -->
              <div
                style='
                display: flex;
                font-family: "Franklin Gothic Medium",
                  "Arial Narrow&quot, Arial, sans-serif;
              '
              >
                <div style="flex: 1">
                  <el-progress
                    striped
                    :stroke-width="6"
                    :percentage="item.percentage"
                  />
                </div>
                <span style="height: 1rem; margin-left: -1rem; z-index: 10">
                  <svg
                    @click="clickCancel(item, index)"
                    t="1718871404428"
                    class="icon"
                    viewBox="0 0 1024 1024"
                    version="1.1"
                    xmlns="http://www.w3.org/2000/svg"
                    p-id="6448"
                    width="20"
                    height="14"
                  >
                    <path
                      d="M850.538343 895.516744c-11.494799 0-22.988574-4.386914-31.763424-13.161764L141.103692 204.669426c-17.548678-17.534352-17.548678-45.992497 0-63.525825 17.548678-17.548678 45.977147-17.548678 63.525825 0l677.671227 677.685553c17.548678 17.534352 17.548678 45.992497 0 63.525825C873.526917 891.128807 862.032118 895.516744 850.538343 895.516744z"
                      fill="#888888"
                      p-id="6449"
                    ></path>
                    <path
                      d="M172.867116 895.516744c-11.494799 0-22.988574-4.386914-31.763424-13.161764-17.548678-17.534352-17.548678-45.992497 0-63.525825l677.671227-677.685553c17.548678-17.548678 45.977147-17.548678 63.525825 0 17.548678 17.534352 17.548678 45.992497 0 63.525825L204.629517 882.354979C195.85569 891.128807 184.360891 895.516744 172.867116 895.516744z"
                      fill="#888888"
                      p-id="6450"
                    ></path>
                  </svg>
                </span>
              </div>
            </div>
          </div>
        </template>
        <template #reference>
          <el-button
            @click="downloadCenter"
            type="primary"
            :icon="Upload"
            size="small"
            circle
          />
        </template>
      </el-popover>
      <!-- 登录的显示 -->
      <div class="LoginAvatar" v-if="userStore.token">
        <el-avatar
          style="margin-left: 10px"
          :size="40"
          :src="
            userStore.userInfo.avatar == ''
              ? backupImg.img
              : wordAllData + userStore.userInfo.avatar
          "
        />
        <el-dropdown trigger="click">
          <span class="el-dropdown-link">
            <div style="display: flex; align-items: center">
              <p style="margin: 10px 10px; width: 100%">
                {{ userStore.userInfo.name || backupImg.name }}
              </p>
              <el-icon class="el-icon--right"><arrow-down /></el-icon>
            </div>
          </span>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item @click="logout">退出登录</el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </div>
      <!-- 没有登录的显示 -->
      <div v-else class="notLoginAvatar" @click="onLogin">登录</div>
    </div>

    <!-- 显示重启或者关机的弹出框 -->
    <el-dialog
      v-model="homestore.isShowShutdownOrReboot"
      width="700"
      @close="closeDialog"
    >
      <div class="shutdown_reboot">
        <el-radio-group v-model="radioValue" @change="changeRadio">
          <el-radio
            :label="item == '0' ? '立即' : item + '分钟'"
            :value="item"
            v-for="(item, index) in shutdownAndRebbotCheckout"
            :key="index"
          />
        </el-radio-group>
      </div>
      <div class="shutdown_reboot_define">
        <div class="item" v-for="(item, index1) in itemList" :key="index1">
          <div class="shutdown_item">{{ item.bectrlEquipName }}</div>
          <div style="display: flex; align-items: center">
            <el-input
              @blur="blurInputEvent(item, index1)"
              v-model="item.timeout"
              style="width: 140px"
              placeholder="选择或输入执行时长"
            >
              <template #suffix>
                <el-icon class="el-input__icon">分钟</el-icon>
              </template>
            </el-input>

            <el-dropdown trigger="click">
              <span class="el-dropdown-link">
                <el-icon class="el-icon--right">
                  <arrow-down />
                </el-icon>
              </span>
              <template #dropdown>
                <el-dropdown-menu>
                  <el-dropdown-item
                    v-for="(item, index) in shutdownAndRebbotCheckout"
                    :key="index"
                    @click="dropDownEvent(item, index1)"
                  >
                    {{ item == "0" ? "立即" : item + "分钟" }}
                  </el-dropdown-item>
                </el-dropdown-menu>
              </template>
            </el-dropdown>
          </div>
        </div>
      </div>
      <template #footer>
        <div class="dialog-footer">
          <el-button type="primary" @click="dialogConfirm">执行</el-button>
          <el-button @click="homestore.isShowShutdownOrReboot = false">
            取消
          </el-button>
        </div>
      </template>
    </el-dialog>

    <!-- 显示文件系统弹出框 -->
    <el-dialog
      @close="homestore.isShowFileSystemDialog = false"
      v-model="homestore.isShowFileSystemDialog"
      title="文件预览"
      width="500"
      height="800"
    >
      <div style="height: 20rem; overflow-y: auto"><FileSystem /></div>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { GET_TOKEN } from "@/utils/token";
import NoticesComponent from "./components/NoticesComponent.vue";
import SettingStore from "@/store/modules/setting";
import { ArrowRight } from "@element-plus/icons-vue";
import {
  RefreshRight,
  Upload,
  Loading,
  Monitor,
  SwitchButton,
} from "@element-plus/icons-vue";
import { useRoute, useRouter } from "vue-router";
import useUserStore from "@/store/modules/user/user";
import useHomeStore from "@/store/modules/home/home";
import { ref, reactive, watch } from "vue";
import { ElMessage } from "element-plus";
import { startShutDownApi } from "@/api/home/index";
import FileSystem from "./components/FileSystem.vue";
const $router = useRouter();
let useSettingStore = SettingStore();
let homestore = useHomeStore();
let wordAllData = import.meta.env.VITE_APP_IMAGE;
let $route = useRoute();
let userStore = useUserStore();
//重启关机单选框
let shutdownAndRebbotCheckout = ["0", "1", "2", "3", "4", "5"];
let radioValue = ref("1");
//所框选的设备列表
let itemList = ref(homestore.ShutdownOrRebootParam);
//点击单选框触发的回调
const changeRadio = (val: any) => {
  // console.log("单选框", val);
  radioValue.value = val;
  //让所有的分钟数等于单选框数
  for (let i = 0; i < itemList.value.length; i++) {
    itemList.value[i].timeout = val;
  }
};

//点击取消
const clickCancel = (_item: any, index: any) => {
  homestore.downLoadList.splice(index, 1);
};
//封装一个判断输入分钟合法的函数
const inputLegal = (val: any) => {
  if (val >= 0 && val <= 5 && typeof val == "number") {
    return true;
  } else if (val < 0 || val > 5) {
    console.log(typeof val);
    ElMessage({
      message: "输入数字必须是0-5之间",
      type: "error",
    });
    return false;
  } else {
    ElMessage({
      message: "输入不合法",
      type: "error",
    });
    return false;
  }
};
//判断输入所有的分钟数是否等于我单选框的值
const checkIsEqualRadioValue = () => {
  //思想是：
  //1. 先遍历所有的元素分钟，封装成一个数组
  let arr = [];
  for (let i = 0; i < itemList.value.length; i++) {
    arr.push(itemList.value[i].timeout);
  }
  //2. 把数组去重，如果去重的个数大于1，就说明所有的分钟数不相等。
  let res: any = [];
  for (let i = 0; i < arr.length; i++) {
    if (res.indexOf(arr[i]) == -1) {
      res.push(arr[i]);
    }
  }
  if (res.length == 1) {
    return {
      isEqual: true,
      value: res[0],
    };
  } else {
    return {
      isEqual: false,
      value: "",
    };
  }
};
//输入框失去焦点触发
const blurInputEvent = (val: any, index: any) => {
  if (!inputLegal(parseInt(val.timeout))) {
    itemList.value[index].timeout = "";
  }
  // if (!checkItemList()) {
  //   radioValue.value = "";
  // }
  let { isEqual, value } = checkIsEqualRadioValue();
  // console.log(isEqual, value);
  if (isEqual) {
    radioValue.value = value;
  } else {
    radioValue.value = "";
  }
};
//点击下拉框获取值和索引
const dropDownEvent = (val: any, index: any) => {
  // console.log("值：", val, "索引：", index);
  itemList.value[index].timeout = val;
  // if (!checkItemList()) {
  //   radioValue.value = "";
  // }
  let { isEqual, value } = checkIsEqualRadioValue();
  // console.log(isEqual, value);
  if (isEqual) {
    radioValue.value = value;
  } else {
    radioValue.value = "";
  }
};
//关闭弹出框
const closeDialog = () => {
  // console.log(111);
  // Object.assign(homestore.ShutdownOrRebootParam, []);
  homestore.ShutdownOrRebootParam = [];
};
//确认重启/关机
const dialogConfirm = async () => {
  for (let i = 0; i < itemList.value.length; i++) {
    if (itemList.value[i].timeout.trim() == "") {
      ElMessage({
        message: "请检查输入情况",
        type: "error",
      });
      return;
    }
  }
  //如果不存在Token,让其登录
  if (!GET_TOKEN()) {
    ElMessage({
      message: "请先登录",
      type: "error",
      duration: 1800,
    });
    setTimeout(() => {
      $router.replace("/newlogin");
    }, 500);
    return;
  }
  //生成要发送的数据包
  let sendParam: any = {};
  sendParam.ctrlerPhone = GET_TOKEN();
  sendParam.type = homestore.MyType;
  sendParam.list = homestore.ShutdownOrRebootParam;
  let res = await startShutDownApi(sendParam);
  console.log("关机重启返回值情况：", res);
  if (res.status == 1) {
    ElMessage({
      type: "success",
      message: homestore.MyType == 1 ? "关机成功" : "重启成功",
      duration: 1500,
    });
  } else {
    ElMessage({
      type: "error",
      message: "操作失败，请稍后再试！",
      duration: 1500,
    });
  }
  homestore.isShowShutdownOrReboot = false;
};
let backupImg = reactive({
  img: "https://ts1.cn.mm.bing.net/th?id=OIP-C.PwfrOCvt8LeHykjwN2ms6gAAAA&w=212&h=212&c=8&rs=1&qlt=90&o=6&pid=3.1&rm=2",
  name: "Test用户",
});
//监听是否点击了关机或者重启，从而进行值初次赋值。
watch(
  () => homestore.isShowShutdownOrReboot,
  () => {
    itemList.value = homestore.ShutdownOrRebootParam;
    for (let i = 0; i < itemList.value.length; i++) {
      itemList.value[i].timeout = radioValue.value;
    }
  },
);
//点击折叠和展开按钮
const changeIcon = () => {
  useSettingStore.fold = !useSettingStore.fold;
};
//点击刷新按钮
const refreshEvent = () => {
  useSettingStore.refresh = !useSettingStore.refresh;
};
//点击登录
let isClick = ref(false);
const onLogin = () => {
  isClick.value = true;
  $router.push("/newlogin");
};
//点击退出登录
const logout = async () => {
  await userStore.userLogout();

  $router.replace("/newlogin");
};
//去客户端中心
const toClientCenter = () => {
  let routeData = $router.resolve({
    name: "download",
  });
  window.open(routeData.href, "_blank");
};

const downloadCenter = () => {
  homestore.isShowDownLoadList = !homestore.isShowDownLoadList;
};
</script>

<style scoped lang="scss">
.el-progress :deep(.el-progress__text) {
  span {
    font-size: 12px;
    // color: red;
  }
}
.list {
  height: 3rem;
  margin-top: 1rem;
}
.tabsListContainer {
  min-height: 5rem;
  overflow-y: auto;
}
.tabsListContainer::-webkit-scrollbar {
  width: 4px;
  // height: 6px;
}
.tabsListContainer::-webkit-scrollbar-track {
  background: rgb(239, 239, 239);
  border-radius: 2px;
}
.tabsListContainer::-webkit-scrollbar-thumb {
  background: #40a0ff49;
  border-radius: 10px;
}
.tabsListContainer::-webkit-scrollbar-thumb:hover {
  background: #40a0ff;
}
::v-deep .el-input__inner {
  &::placeholder {
    // color: red;
    font-size: 10px;
  }
}

.el-button--small.is-circle {
  color: white;
}
.shutdown_reboot {
  text-align: center;
}
.shutdown_reboot_define {
  margin-top: 1.2rem;
  max-height: 21rem;
  overflow-y: auto;
  overflow-x: hidden;
  display: grid;
  grid-template-columns: repeat(3, 3fr);
  grid-gap: 20px;

  .item {
    display: flex;
    align-items: center;
    justify-content: space-around;
  }
}
.tabbar {
  width: 100%;
  height: $header;
  display: flex;
  justify-content: space-between;
  align-items: center;
  overflow: hidden;

  .tabbar_left {
    flex: 1;
    .leftContent {
      // background-color: red;
      overflow: hidden;
      margin-left: 20px;
      width: $leftContent;

      .icon {
        display: inline-block;
      }

      .bread {
        display: inline-block;
      }
    }
  }

  .tabbar_center {
    flex: 2;
    min-width: 15.2rem;
    height: 100%;
    // background-color: red;
    margin-right: 1rem;
  }

  .tabbar_right {
    flex: 1;
    margin-right: 10px;
    display: flex;
    align-items: center;
    justify-content: end;
    height: 100%;
    .LoginAvatar {
      display: flex;
      align-items: center;
      flex-wrap: no-wrap;
    }
    .notLoginAvatar {
      margin-left: 2rem;
      width: calc($header / 1.3);
      height: calc($header / 1.3);
      border: 1px solid rgb(255, 255, 255);
      background-color: rgb(232, 232, 237);
      border-radius: 100%;
      text-align: center;
      font-size: 0.7rem;
      font-family: "Courier New", Courier, monospace;
      font-family: sans-serif;
      line-height: calc($header / 1.3);
    }
  }
}
</style>
