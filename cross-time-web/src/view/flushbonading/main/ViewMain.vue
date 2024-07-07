<template>
  <router-view v-slot="{ Component }">
    <transition name="fade">
      <!-- 渲染layout -->
      <component v-if="flag" :is="Component" />
    </transition>
  </router-view>
</template>

<script setup lang="ts">
import { ref } from "vue";
let flag = ref(true);
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
