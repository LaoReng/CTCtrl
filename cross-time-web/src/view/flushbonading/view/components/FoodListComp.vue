<!-- 复用表格 -->
<template>
  <div class="food_list">
    <el-table :max-height="size" :data="tableData">
      <!-- 食物名字 -->
      <el-table-column prop="Name" label="NAME" />
      <!--fresh degree新鲜度-->
      <el-table-column prop="Fd" label="FD" />
      <!-- 数量 -->
      <el-table-column prop="Num" label="NUM" />
    </el-table>
  </div>
</template>

<script setup lang="ts">
// import { ref } from "vue";
defineProps(["tableData", "size"]);
</script>

<style scoped></style>
