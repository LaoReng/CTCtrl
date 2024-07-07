<template>
  <div class="logoContainer">
    <img :src="setting.logo" width="72rem" height="100%" />
    <div v-show="!fold.fold" class="logoName">{{ setting.title }}</div>
  </div>
</template>

<script setup>
import SettingStore from "@/store/modules/setting";
import setting from "@/utils/setting";
const fold = SettingStore();
</script>

<style lang="scss" scoped>
.logoContainer {
  padding: $logoPadding/2;
  height: $logoContainerHeight;
  color: white;
  font-size: 1rem;
  font-weight: 700;
  display: flex;
  justify-content: center;
  align-items: center;

  img {
    border-radius: 100%;
  }

  .logoName {
    margin-left: 0.4rem;
  }
}
</style>
