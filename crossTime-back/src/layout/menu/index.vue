<template>
  <template v-for="item in MenuList" :key="item.path">
    <!-- 没有子路由 -->
    <template v-if="!item.children">
      <!-- 如果要求不显示 -->
      <el-menu-item
        v-if="!item.meta.hidden"
        :index="item.path"
        @click="goRoute"
      >
        <el-icon>
          <component :is="item.meta.icon"></component>
        </el-icon>
        <template #title>
          <span>{{ item.meta.title }}</span>
        </template>
      </el-menu-item>
    </template>

    <!-- 只有一个子路由 -->
    <template v-if="item.children && item.children.length == 1">
      <el-menu-item
        v-if="!item.meta.hidden"
        :index="item.children[0].path"
        @click="goRoute"
      >
        <el-icon>
          <component :is="item.children[0].meta.icon"></component>
        </el-icon>
        <template #title>
          <span>{{ item.children[0].meta.title }}</span>
        </template>
      </el-menu-item>
    </template>
    <!-- 大于一个子路由 -->
    <el-sub-menu
      v-if="item.children && item.children.length > 1"
      :index="item.path"
    >
      <template #title>
        <el-icon>
          <component :is="item.meta.icon"></component>
        </el-icon>
        <span>{{ item.meta.title }}</span>
      </template>
      <!-- 递归组件 -->
      <MyMenu :MenuList="item.children"></MyMenu>
    </el-sub-menu>
  </template>
</template>

<script setup lang="ts">
import { useRouter } from "vue-router";
let $router = useRouter();
//获取父组件传递过来的全部路由数组
defineProps(["MenuList"]);

const goRoute = (vc: any) => {
  // console.log(vc.index);
  $router.push({
    path: vc.index,
  });
};
</script>
<!-- 递归组件 -->
<script lang="ts">
export default {
  name: "MyMenu",
};
</script>

<style scoped></style>
