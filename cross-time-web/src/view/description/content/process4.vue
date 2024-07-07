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
<h1 align="center">安装客户端</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>双击下载好的客户端程序，进行被控端程序安装</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片9.png"}">
  <li>选择程序安装类型,点击下一步</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片10.png"}">
  <li>查看程序安装说明文件,点击下一步</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片11.png"}">
  <li>选择程序安装位置并同意协议约定条目,点击下一步</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片12.png"}">
  <li>按需求选择要安装的功能并点击下一步</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片13.png"}">
  <li>点击安装等到进度条结束完成安装</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片14.png"}">
  </ol>
`;
</script>
