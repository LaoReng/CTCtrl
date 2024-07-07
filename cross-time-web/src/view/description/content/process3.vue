<template>
  <v-md-editor
    mode="preview"
    v-model="text"
    left-toolbar="undo redo | tip"
    height="100%"
  />
</template>

<script setup lang="ts">
const text = `
<h1 align="center">下载客户端</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>可以通过登录页面右下角的下载中心或者平台菜单栏的客户端下载按钮进入下载中心</li>
  <img src="${import.meta.env.VITE_APP_IMAGE}/图片6.png"}">
  <img src="${import.meta.env.VITE_APP_IMAGE}/图片7.png"}">
  <li>点击下载即可下载客户端</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片8.png"}">
  </ol>
`;
</script>
