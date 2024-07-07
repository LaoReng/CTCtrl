<template>
  <v-md-editor mode="preview" v-model="text" height="100%" />
</template>

<script setup lang="ts">
const text = `
<h1 align="center">注册</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol> 
  <li>用户首先输入昵称,手机号和密码注册账号</li>
  <img src="${import.meta.env.VITE_APP_IMAGE}/图片1.png"}">
  <li>点击确定后会跳转到问题设置页面,用户需要设置两个问题用于忘记密码输入答案来找回密码,设置完问题后点击注册即可完成注册</li>
<img src="${import.meta.env.VITE_APP_IMAGE}/图片2.png"}">
  </ol>
`;
console.log(import.meta.env.VITE_APP_IMAGE);
</script>
