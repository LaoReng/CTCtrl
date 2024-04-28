<template>
  <el-menu
    background-color="#353743"
    text-color="#fff"
    active-text-color="#53a8ff"
    :default-active="currentIndex"
    @select="getSelectIndex"
  >
    <el-menu-item index="/user" route="/user">
      <i class="el-icon-user"></i>
      <span>用户管理</span>
    </el-menu-item>
    <el-menu-item index="/device" route="/device">
      <i class="el-icon-monitor"></i>
      <span>设备管理</span>
    </el-menu-item>
    <el-menu-item index="/notificat" route="/notificat">
      <i class="el-icon-coordinate"></i>
      <span>通知公告</span>
    </el-menu-item>
  </el-menu>
</template>

<script>
export default {
  name: "AppMenu",
  data() {
    return {
      currentIndex: "",
    };
  },
  methods: {
    getSelectIndex(index) {
      this.currentIndex = index;
      this.$router.push(index).catch((err) => {
        // 防止重复跳转导致的错误，捕获 NavigationDuplicated 异常并忽略
        if (err.name !== "NavigationDuplicated") {
          throw err;
        }
      });
    },
    updateCurrentIndex() {
      this.currentIndex = this.$route.fullPath;
    },
  },
  created() {
    this.updateCurrentIndex();
    this.$router.afterEach(() => {
      this.updateCurrentIndex();
    });
  },
};
</script>

<style></style>
