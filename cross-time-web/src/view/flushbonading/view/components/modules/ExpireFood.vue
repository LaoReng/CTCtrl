<!-- 过期食物列表情况 -->
<template>
  <FoodListComp :tableData="tableData" :size="500" />
</template>

<script setup lang="ts">
import FoodListComp from "../FoodListComp.vue";
const tableData = [
  {
    Name: "安慕希",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "AD钙",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "农夫山泉",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "山楂",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "旺旺碎冰冰",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
  {
    Name: "脆脆肠",
    Fd: "65%",
    Num: Math.floor(Math.random() * 100),
  },
];
</script>

<style scoped></style>
