<!-- eslint-disable @typescript-eslint/no-unused-vars -->
<template>
  <div class="vncContainer" v-if="isShow">
    <!-- VNC头部 -->
    <div class="header">
      <VncHeader />
    </div>
    <!-- VNC身体 -->
    <div class="view">
      <!-- 显示区域 -->
      <div class="show">
        <!-- <img class="screen" src="@/assets/images/vnc.png" /> -->
        <img class="screen" :src="imgUrl" />
      </div>
      <!-- 控制区域 -->
      <div class="ctrl">
        <el-checkbox-group
          @change="changeBox"
          fill="#585eaa"
          v-model="checkboxGroup"
        >
          <el-checkbox-button
            v-for="(item, index) in event"
            :value="item.name"
            :key="index"
          >
            <img :src="item.icon" />
          </el-checkbox-button>
        </el-checkbox-group>
      </div>
    </div>
  </div>
  <div v-else></div>
</template>

<script setup lang="ts">
import { ElMessage } from "element-plus";
import VncHeader from "./components/VncHeader.vue";
import { onMounted, ref, reactive } from "vue";
// import LimitCtrl from "@/utils/limitCtrl";

import { useRoute, useRouter } from "vue-router";
import useVncStore from "@/store/modules/vnc/vnc";
//仓库
let vncstore = useVncStore();
//路由
let $router = useRouter();
let $route = useRoute();
let checkboxGroup = ref<string[]>([]);
let event = reactive([
  {
    name: "鼠标",
    icon: new URL("@/assets/images/mouse.png", import.meta.url).href,
    isChecked: false,
  },
  {
    name: "键盘",
    icon: new URL("@/assets/images/keyword.png", import.meta.url).href,
    isChecked: false,
  },
]);

//初始化一个图片
// eslint-disable-next-line @typescript-eslint/no-unused-vars
let imgUrl = ref("");
//是否初始化显示页面
let isShow = ref<boolean>(true);
//鼠标锁，键盘锁
let miceFlag = ref<boolean>(false);
let keyFlag = ref<boolean>(false);

//定时器
//改变鼠标键盘的点击回调
const changeBox = () => {
  //鼠标键盘同时有
  if (
    checkboxGroup.value.includes("鼠标") == true &&
    checkboxGroup.value.includes("键盘") == true
  ) {
    if (keyFlag.value == false) {
      vncstore.addKeyWordDownHandler();
      vncstore.addKeyWordUpHandler();
      keyFlag.value = true;
    }
    if (miceFlag.value == false) {
      vncstore.addClickDownHandler();
      vncstore.addClickUpHandler();
      vncstore.addMouseMoveHandler();
      vncstore.addMouseScrollHander();
      miceFlag.value = true;
    }
  } else if (
    checkboxGroup.value.includes("鼠标") == true &&
    checkboxGroup.value.includes("键盘") == false
  ) {
    // 没有键盘
    if (keyFlag.value == true) {
      vncstore.removeKeyWordDownHandler();
      vncstore.removeKeyWordUpHandler();
      // this.removeMouseMoveHandler();
      keyFlag.value = false;
    }
    if (miceFlag.value == false) {
      vncstore.addClickDownHandler();
      vncstore.addClickUpHandler();
      vncstore.addMouseMoveHandler();
      vncstore.addMouseScrollHander();
      miceFlag.value = true;
    }
  } else if (
    checkboxGroup.value.includes("键盘") == true &&
    checkboxGroup.value.includes("鼠标") == false
  ) {
    // 只有键盘
    if (miceFlag.value == true) {
      vncstore.removeClickDownHandler();
      vncstore.removeMouseMoveHandler();
      vncstore.removeClickUpHandler();
      vncstore.removeMouseScrollHandler();
      miceFlag.value = false;
    }
    if (keyFlag.value == false) {
      vncstore.addKeyWordDownHandler();
      // this.addMouseMoveHandler();
      vncstore.addKeyWordUpHandler();
      keyFlag.value = true;
    }
  } else {
    if (keyFlag.value == true) {
      vncstore.removeKeyWordDownHandler();
      vncstore.removeKeyWordUpHandler();
      keyFlag.value = false;
    }
    if (miceFlag.value == true) {
      vncstore.removeClickDownHandler();
      vncstore.removeClickUpHandler();
      vncstore.removeMouseScrollHandler();
      vncstore.removeMouseMoveHandler();
      miceFlag.value = false;
    }
  }
};

//初始化
onMounted(() => {
  document.addEventListener("contextmenu", function (event) {
    event.preventDefault();
  });
  //limit函数（）
  let query = $route.query;
  // console.log(query);
  //这里是获取设备的信息，这些信息存储在了本地里
  const getInfo = JSON.parse(localStorage.getItem("startEquipment") as string);
  console.log(getInfo);
  /*
  CtrlTime: 300000
  eStatus: 1
  height: 1080
  imageDir: "user1-sb1"
  width: 1920
  */
  //如果获取不到
  if (!getInfo) {
    ElMessage({
      type: "error",
      message: "参数错误",
      showClose: true,
      duration: 1000,
    });
    $router.replace("/home");
    return;
  }
  console.log("刷新当前时间为：", getInfo.CtrlTime);
  //设置定时器
  // vncstore.deadLine = Date.now() + 1000 * 60 * parseInt(getInfo.CtrlTime);
  vncstore.deadLine = Date.now() + parseInt(getInfo.CtrlTime);
  //封装一个对方屏幕的大小，进行鼠标精准控制
  let currentParam = { width: getInfo.width, height: getInfo.height };
  vncstore.imgParam = { ...currentParam };
  vncstore.baseInfo = { ...getInfo, ...query };
  // console.log(vncstore.baseInfo);
  /*
        CtrlTime: "10"
        eStatus: 1
        imageDir: "user1-sb1"
        name: "sb1"
        */
  //定时器逻辑
  vncstore.timer = setInterval(() => {
    // console.log(import.meta);
    imgUrl.value = `${import.meta.env.VITE_APP_URL_IMAGE}/image/${vncstore.baseInfo.imageDir}.jpeg?timer=${Date.now()}`;
    // imgUrl.value = `${import.meta.env.VITE_APP_URL_IMAGE}/getImage?name=${vncstore.baseInfo.imageDir}&timer=${Date.now()}`;
    // imgUrl.value = `${import.meta.env.VITE_APP_URL_IMAGE}/imageApi/image/1.jpg?timer=${Date.now()}`;
    // console.log(imgUrl.value);
  }, 150);
  isShow.value = true;
});
</script>

<style scoped lang="scss">
.vncContainer {
  overflow: hidden;
  -webkit-user-drag: none;
  .header {
    width: 100vw;
    height: $vnc-header;
  }

  .view {
    -webkit-user-drag: none;
    width: 100vw;
    height: calc(100vh - $vnc_header);
    // background-color: blue;
    display: flex;

    .show {
      // background-color: red;
      width: calc(100vw - $vnc_width);
      height: calc(100vh - $vnc_header);
      img {
        -webkit-user-drag: none;
        width: 100%;
        height: 100%;
      }
    }

    .ctrl {
      width: $vnc_width;
      flex-wrap: wrap;
      display: flex;
      flex-direction: column;

      img {
        -webkit-user-drag: none;
        width: 100%;
        height: 100%;
      }
    }
  }
}
</style>
