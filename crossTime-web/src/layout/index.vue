<template>
  <div class="container"> <!-- 使用 vh 单位使容器高度等于视窗高度 -->
    <el-header>
      <!-- 去掉竖线 -->
      <AppHeader style="caret-color: rgba(0, 0, 0, 0)" />
    </el-header>
    <div class="content">
      <el-aside width="12rem">
        <AppMenu style="caret-color: rgba(0, 0, 0, 0)" />
      </el-aside>
      <div class="main">
        <!-- TabView -->
        <router-view></router-view>
      </div>
    </div>
  </div>
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
  data() {
    return {

    }
  },
  mounted() {

  }
};
</script>

<style scoped>
.container {
  /* max-height: 100%; */

  /* background-color: rgb(20, 135, 78); */
  display: flex;
  flex-direction: column;
}

.container::-webkit-scrollbar {
  display: none
}

.el-header {
  background-color: rgb(72, 72, 72);
  /* padding: 0; */
  position: fixed;
  top: 0;
  left: 0;
  /* 这里不能设置布局层级的问题，因为会遮挡到提示窗，但是如果不加这个会影响到滚动盒子 */
  z-index: 10;
  width: 100%;
  min-width: 50rem;
}

.content {
  display: flex;
  height: 100%;
  margin-top: 60px;
  min-width: 50rem;
}

.content .el-aside {
  min-height: calc(100vh - 60px);
  background-color: #353743;
  position: fixed;
  z-index: 10;
  left: 0;
  min-width: 11rem;

}


.main {
  margin-left: 13rem;
  padding: 0px;
  /* 当内容高度超过元素本身时出现垂直滚动条 */
  /* overflow-y: auto; */
  /* display: flex;
  align-items: center;
  justify-content: center; */
  /* width: 100%; */
  min-width: 50rem;
}
</style>
