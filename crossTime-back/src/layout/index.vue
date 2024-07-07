<template>
  <div class="layout-container">
    <!-- 左侧 -->
    <div class="slider" :class="{ fold: settingStore.fold ? true : false }">
      <!-- logo -->
      <Logo style="margin-left: -30px" />
      <!-- 展示菜单 -->
      <el-scrollbar class="scrollbar">
        <el-menu
          :collapse="settingStore.fold ? true : false"
          :default-active="$route.path"
          active-text-color="#ffd04b"
          background-color="#001529"
          text-color="#fff"
        >
          <!-- 根据路由动态生成菜单 -->
          <Menu :MenuList="UseAuthStore.menuRoutes" />
        </el-menu>
      </el-scrollbar>
    </div>
    <!-- 顶部导航 -->
    <div class="header" :class="{ fold: settingStore.fold ? true : false }">
      <Tabbar />
    </div>

    <!-- 内容 -->
    <div class="content" :class="{ fold: settingStore.fold ? true : false }">
      <Main></Main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { useRoute } from "vue-router";
import Logo from "./logo/index.vue";
import Menu from "./menu/index.vue";
import Main from "./main/index.vue";
import Tabbar from "./tabbar/index.vue";
import SettingStore from "@/store/modules/setting.ts";
//获取用户相关的仓库
import useAuthStore from "@/store/auth.ts";
let UseAuthStore = useAuthStore();
let settingStore = SettingStore();

let $route = useRoute();
console.log($route);
</script>

<style scoped lang="scss">
.layout-container {
  width: 100%;
  height: 100vh;
  overflow: hidden;
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
      //这里-4是因为我们设置了内边距
      height: calc(100vh - $logoContainerHeight - $logoPadding);
      position: absolute;
      width: 100%;
      top: $logoContainerHeight + $logoPadding;
      // background-color: aliceblue;

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
    // background: rgb(233, 237, 241);

    &.fold {
      width: calc(100% - $foldWidth);
      left: $foldWidth;
    }
  }

  .content {
    background: rgb(233, 237, 241);
    overflow: auto;
    box-sizing: border-box;
    position: absolute;
    left: $slider-width;
    top: $header;
    width: calc(100% - $slider-width);
    height: calc(100% - $header);
    // background-color: rgb(30, 0, 255);
    padding: 1rem;
    transition: all 0.4s;
    &.fold {
      width: calc(100% - $foldWidth);
      left: $foldWidth;
    }
  }
}
</style>
