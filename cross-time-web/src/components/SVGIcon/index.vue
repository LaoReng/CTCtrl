<template>
  <div>
    <svg :style="{ width, height }">
      <use :xlink:href="prefix + name" :fill="color"></use>
    </svg>
  </div>
</template>

<script setup lang="ts">
//接收父组件传递过来的参数
defineProps({
  //xlink:href的前缀
  prefix: {
    type: String,
    default: "#icon-",
  },
  //接收父组件传递图标名字
  name: String,
  //接收父组件传递的颜色
  color: {
    type: String,
    default: "",
  },
  //接收父组件传递过来的高度和宽度
  width: {
    type: String,
    default: "16px",
  },
  height: {
    type: String,
    default: "16px",
  },
});
</script>

<style scoped></style>
