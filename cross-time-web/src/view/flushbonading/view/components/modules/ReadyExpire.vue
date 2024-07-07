<!-- 快过期的食物 -->
<template>
  <FoodListComp :tableData="tableData" :size="500" />
</template>

<script setup lang="ts">
import FoodListComp from "../FoodListComp.vue";
const tableData = [
  {
    Name: "橘子",
    Fd: "95%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "红富士",
    Fd: "85%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "血橙",
    Fd: "75%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "菠萝",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
];
</script>

<style scoped></style>
