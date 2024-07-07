<template>
  <div class="container">
    <div class="left">
      <div class="title">{{ setting.title }}使用说明文档</div>
      <div class="date">更新时间:2024-06-25 23:47:13</div>
    </div>
    <div class="right">
      <div class="img">
        <el-avatar :size="100">
          <img src="@/assets/images/logo.png" />
        </el-avatar>
      </div>
      <div class="name">CTCtrl开发团队</div>
    </div>
  </div>
</template>

<script setup lang="ts">
import setting from "@/utils/setting";
</script>

<style scoped lang="scss">
.container {
  height: 100%;
  display: flex;
  align-items: center;
  .left {
    display: flex;
    flex-direction: column;
    justify-content: space-around;
    flex: 1;
    height: 75%;
    .title {
      font-size: 2.3rem;
      font-weight: bold;
    }
    .date {
      color: #ccc;
    }
  }
  .right {
    height: 80%;
    flex: 0.2;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: space-around;
    font-weight: 900;
  }
}
</style>
