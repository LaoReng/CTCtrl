<template>
  <router-view v-slot="{ Component }">
    <transition name="fade">
      <!-- 渲染layout -->
      <component v-if="flag" :is="Component" />
    </transition>
  </router-view>
</template>

<script setup lang="ts">
//获取settingStore
import SettingStore from "@/store/modules/setting";
import { nextTick, ref, watch } from "vue";
let store = SettingStore();
//控制当前组件是否销毁重建
let flag = ref(true);
//监听仓库内的刷新按钮变化，如果变化了，就刷新当前页面
watch(
  () => store.refresh,
  () => {
    flag.value = false;
    //当响应式数据发生变化的时候，可以获取到更新后的dom
    nextTick(() => {
      flag.value = true;
    });
  },
);
</script>

<style scoped lang="scss">
.fade-enter-from {
  opacity: 0;
  /* transform: scale(0); */
}
.fade-enter-active {
  transition: all 1s;
}
.fade-enter-to {
  opacity: 1;
  /* transform: scale(1); */
}
</style>
