<template>
  <div class="tabbar">
    <div class="tabber_left">
      <div class="leftContent">
        <el-icon style="margin-right: 10px" @click="changeIcon">
          <component :is="useSettingStore.fold ? 'Fold' : 'Expand'"></component>
        </el-icon>
        <!-- 左侧面包屑 -->
        <el-breadcrumb :separator-icon="ArrowRight">
          <el-breadcrumb-item
            v-for="(item, index) in $route.matched"
            :key="index"
            :to="item.path"
            v-show="item.meta.title"
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
    <div class="tabber_right">
      <el-button
        @click="refreshEvent"
        type="primary"
        :icon="RefreshRight"
        size="small"
        circle
      />
      <!-- <el-button
        @click="FullScreenEvent"
        type="primary"
        :icon="FullScreen"
        size="small"
        circle
      /> -->

      <!-- <el-avatar
        style="margin-left: 10px; margin-right: -20px"
        :size="40"
        src="src\assets\images\avatar.jpg"
      /> -->
      <el-dropdown trigger="click">
        <span class="el-dropdown-link">
          <div style="display: flex; align-items: center">
            <el-button type="primary" size="small" circle style="margin-left: 12px;"
              ><el-icon><SwitchButton /></el-icon
            ></el-button>
          </div>
        </span>
        <template #dropdown>
          <el-dropdown-menu>
            <el-dropdown-item @click="logout" :icon="Opportunity">
              退出登录
            </el-dropdown-item>
          </el-dropdown-menu>
        </template>
      </el-dropdown>
      <p style="margin: 10px 10px">{{ phone }}</p>
    </div>
    <!-- 显示抽屉 -->
    <!-- <el-drawer size="20%" v-model="changeColor" title="控制面板">
      <div class="drawer">
        <el-switch
          v-model="dark"
          @change="changeBtn"
          active-text="黑暗模式"
          inactive-text="白天模式"
        />
      </div>
    </el-drawer> -->
  </div>
</template>

<script setup lang="ts">
import SettingStore from "@/store/modules/setting";
import { ArrowRight } from "@element-plus/icons-vue";
import { useAuthStore } from "@/store/auth";

import {
  Opportunity,
  RefreshRight,
} from "@element-plus/icons-vue";
import { useRoute, useRouter } from "vue-router";
import { ref } from "vue";
const $router = useRouter();
const authStore = useAuthStore();
const phone = ref(authStore.token);
let useSettingStore = SettingStore();
let $route = useRoute();
// let changeColor = ref(false);
// let dark = ref<boolean>(false);
//点击折叠和展开按钮
const changeIcon = () => {
  useSettingStore.fold = !useSettingStore.fold;
};
//点击刷新按钮
const refreshEvent = () => {
  useSettingStore.refresh = !useSettingStore.refresh;
};
//全屏按钮点击回调
// const FullScreenEvent = () => {
//   //dom的一个属性，可以用来判断是否是全屏，如果是全屏返回true，反之返回null
//   let full = document.fullscreenElement;
//   if (!full) {
//     //利用根节点的方法全屏模式
//     document.documentElement.requestFullscreen();
//   } else {
//     document.exitFullscreen();
//   }
// };
//点击退出登录
const logout = async () => {
  await authStore.logout();
  //记录当前的所处位置下次直接从这个位置进行跳转
  $router.push({
    path: "/login",
    query: {
      redirect: $route.path,
    },
  });
};
// //改变背景颜色
// const changeBg = () => {
//   changeColor.value = !changeColor.value;
// };
// //点击改变颜色按钮
// const changeBtn = () => {
//   let html = document.documentElement;
//   console.log(html);
//   //判断html是否有dark
//   dark.value ? (html.className = "dark") : (html.className = "");
// };
</script>

<style scoped lang="scss">
.tabbar {
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: space-between;
  align-items: center;

  .leftContent {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-left: 20px;
  }

  .tabber_right {
    margin-right: 10px;
    display: flex;
    align-items: center;
  }
}
</style>
