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
<h1 align="center">如何控制设备</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>安装完客户端后,用户在客户端进行登录,登录后此设备就会自动归属于该用户的设备</li>
  <img src="${import.meta.env.VITE_APP_IMAGE}/图片15.png"}">
  <li>被控者可以在被控端的软件托盘右键点击查看控制码获取连接密钥</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片16.png"}">
  <li>双击或者右键点击连接就会弹出密钥框</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片17.png"}">
  <li>输入正确的密钥,点击确定</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片18.png"}">
  <li>验证成功后即可进入该设备的监控页面</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片19.png"}">
  </ol>
`;
</script>
