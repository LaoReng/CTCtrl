<template>
  <div class="layout-container">
    <!-- 左侧 -->
    <div
      class="slider"
      ref="mySlider"
      :class="{ fold: settingStore.fold ? true : false }"
    >
      <!-- logo -->
      <Logo />
      <!-- 展示菜单 -->
      <el-scrollbar class="scrollbar">
        <el-menu
          :collapse="settingStore.fold ? true : false"
          :default-active="$route.path"
          active-text-color="#ffd04b"
          background-color="#545c64"
          text-color="#fff"
        >
          <!-- 根据路由动态生成菜单 -->
          <Menu :MenuList="userStore.menuRoutes"></Menu>
        </el-menu>
      </el-scrollbar>
    </div>
    <!-- 顶部导航 -->
    <div class="header" :class="{ fold: settingStore.fold ? true : false }">
      <!-- <Tabbar @isDialog="getDialogValue" /> -->
      <Tabbar />
    </div>

    <!-- 内容 -->
    <div class="content" :class="{ fold: settingStore.fold ? true : false }">
      <Main></Main>
    </div>
  </div>
</template>

<script setup lang="ts">
// import RegisterVue from "@/view/register/index.vue";
// import LoginVue from "@/view/login/index.vue";
import { useRoute } from "vue-router";
import { onMounted, ref } from "vue";
import Logo from "./logo/index.vue";
import Menu from "./menu/index.vue";
import Main from "./main/index.vue";
import Tabbar from "./tabbar/index.vue";
import SettingStore from "@/store/modules/setting";
// import { ref } from "vue";
//获取用户相关的仓库
import useUserStore from "@/store/modules/user/user";
let userStore = useUserStore();
let settingStore = SettingStore();
let $route = useRoute();
// let showDialog = ref(false);
let mySlider: any = ref(null);

onMounted(() => {
  //禁止浏览器弹出默认菜单的行为
  document.addEventListener("contextmenu", (e) => {
    e.preventDefault();
  });
  // //判断当前是否是登陆状态
  // let status = settingStore.isLogin;
  // console.log(status);
  // //如果当前状态是未登录，则展示弹出框
  // if (status == false) {
  //   showDialog.value = !status;
  // }
});
</script>

<style scoped lang="scss">
.img {
  position: absolute;
  bottom: 1rem;
  left: 1rem;
}
.container {
  margin: 0 auto;
  width: 70%;
}

.layout-container {
  // background-color: red;
  width: 100vw;
  height: 100vh;
  // 去除竖线
  -webkit-user-select: none;
  /*谷歌 /Chrome*/
  -moz-user-select: none;
  /*火狐/Firefox*/
  -ms-user-select: none;
  /*IE 10+*/
  user-select: none;
  .slider {
    position: relative;
    box-sizing: border-box;
    width: $slider-width;
    background-color: $slider-bg;
    height: 100vh;
    transition: all 0.4s;

    &.fold {
      width: $foldWidth;
      text-align: center;
    }

    .scrollbar {
      //去掉竖线
      caret-color: rgba(0, 0, 0, 0);
      //这里-4是因为我们设置了内边距
      height: calc(100vh - $logoContainerHeight - $logoPadding);
      position: absolute;
      width: 100%;
      top: $logoContainerHeight + $logoPadding;

      .el-menu {
        border-right: none;
      }
    }
  }

  .header {
    position: absolute;
    top: 0;
    left: $slider-width;
    // background-color: rgb(215, 250, 223);
    width: calc(100% - $slider-width);
    height: $header;
    transition: all 0.4s;

    &.fold {
      width: calc(100% - $foldWidth);
      left: $foldWidth;
    }
  }

  .content {
    overflow: auto;
    box-sizing: border-box;
    position: absolute;
    left: $slider-width;
    top: $header;
    width: calc(100% - $slider-width);
    height: calc(100% - $header);
    padding: 1.1rem;
    transition: all 0.4s;

    &.fold {
      width: calc(100% - $foldWidth);
      left: $foldWidth;
    }
  }
}
</style>
