<template>
  <el-container>
    <el-header>
      <AppHeader />
    </el-header>
    <el-container>
      <el-aside width="15rem">
        <AppMenu />
      </el-aside>
      <el-main>
        <!-- TabView -->
        <router-view></router-view>
      </el-main>
    </el-container>
  </el-container>
</template>

<script>
// 引入头组件
import AppHeader from "./components/AppHeader.vue";
// 引入menu组件
import AppMenu from "./components/AppMenu.vue";
export default {
  name: "Layout",
  components: {
    AppHeader,
    AppMenu,
  },
};
</script>

<style scoped>
.el-header {
  background-color: rgb(72, 72, 72);
  padding: 0;
}

.el-aside {
  min-height: calc(100vh - 60px);
  background-color: #353743;
}

.el-main {
  padding: 0px;
}
</style>
