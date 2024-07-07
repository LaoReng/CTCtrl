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
<h1 align="center">登录</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>输入手机号/用户名和密码进行登录,如果忘记密码,可以点击忘记密码跳转到获取问题页面</li>
  <img src="${import.meta.env.VITE_APP_IMAGE}/图片3.png"}">
  <li>输入手机号/用户名,点击获取问题按钮跳转到问题验证页面</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片4.png"}">
  <li>输入两个问题的答案,点击提交即可跳转到用户登录页面,并且密码会自动填充</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片5.png"}">
  </ol>
`;
</script>
