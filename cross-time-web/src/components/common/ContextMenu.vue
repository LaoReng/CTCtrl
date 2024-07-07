<template>
  <teleport to="body">
    <div
      v-if="contextMenuVisible"
      class="right-click-menu"
      :style="{ top: `${top}px`, left: `${left}px` }"
    >
      <ul v-for="(item, index) in menuList" :key="index">
        <li @click="handleItemClick(item)">{{ item }}</li>
      </ul>
    </div>
  </teleport>
</template>

<script setup lang="ts">
//props接收参数
//top,left接收右键菜单的弹出坐标
//handleItemClick父组件传入一个方法，实现子组件向父组件传递点击的是哪个菜单
//contextMenuVisible控制这个右键菜单是否显示
//menuList传入一个菜单列表
defineProps([
  "top",
  "left",
  "handleItemClick",
  "contextMenuVisible",
  "menuList",
]);
</script>

<style scoped lang="scss">
.right-click-menu {
  position: fixed;
  background-color: #fff;
  box-shadow: 0 15px 30px rgba(0, 0, 0, 0.3);
  z-index: 1000;
  border-radius: 10px;
  width: 118px;
  font-size: 14px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
}

ul {
  list-style-type: none;
  padding: 0;
  margin: 0;
  text-indent: 18px;
  // background-color: red;
  display: flex;
  justify-content: center;
  align-items: center;
}

li {
  // padding: 8px;
  width: 110px;
  height: 40px;
  cursor: pointer;
  border-radius: 10px;
  display: flex;
  align-items: center;
}

li:hover {
  background-color: #f3f4f6;
}
</style>
