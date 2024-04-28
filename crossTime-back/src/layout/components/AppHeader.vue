<template>
  <div class="app-header">
    <div class="left">
      <img src="../../assets/logo.jpg" style="width: 5rem" />
      <span :style="{ margin: '6px' }">后台管理系统</span>
    </div>
    <div class="right">
      <el-dropdown
        size="small"
        split-button
        trigger="click"
        @command="onSelect"
      >
        超级管理员
        <el-dropdown-menu slot="dropdown">
          <!-- <el-dropdown-item command="profile" route="/user"
            >个人中心</el-dropdown-item
          >
          <el-dropdown-item command="profile-setting"
            >个人设置</el-dropdown-item
          > -->
          <el-dropdown-item command="logout">退出登录</el-dropdown-item>
        </el-dropdown-menu>
      </el-dropdown>

      <div class="btn" @click="onShutDown">
        <i class="el-icon-switch-button"></i>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "AppHeader",
  methods: {
    onSelect(command) {
      // console.log(command);
      if (command == "logout") {
        this.$router.push("/login");
      }
    },
    onShutDown() {
      this.$router.push("/login");
    },
  },
};
</script>

<style scoped>
.app-header {
  color: #fff;
  height: 100%;
  display: flex;
  align-items: center;
  font-size: 20px;
  padding: 0;
}

.app-header .left {
  width: 300px;
  display: flex;
  align-items: center;
}

.app-header .right {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: flex-end;
  text-align: right;
}

.btn {
  margin-left: 1rem;
  margin-right: 1.5rem;
}

.el-dropdown {
  color: white;
}

.el-dropdown:hover {
  color: blue;
}

.el-header {
  width: 100vw;
}
</style>
