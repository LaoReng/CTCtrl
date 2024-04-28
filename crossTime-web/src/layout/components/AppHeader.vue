<template>
  <div class="app-header">
    <div class="left">
      <img src="../../assets/logo.jpg" style="width: 5rem" />
      <span :style="{ margin: '4%' }">跨时空控制台</span>
    </div>

    <div class="center">
      <NoticesComponent v-if="isShow" :currentNotify="currentNotify" />
    </div>
    <div class="right">
      <el-dropdown @command="onSelect">
        <div style="display: flex;align-items: center;">
          <el-avatar
            src="https://ts2.cn.mm.bing.net/th?id=OIP-C.9hA8vMz6W3sKhZWAg_5s0QHaHb&w=249&h=250&c=8&rs=1&qlt=90&o=6&pid=3.1&rm=2"></el-avatar>
          <div style="margin-left: 10px;color:#ffffff">{{ userInfo.name || " " }}</div>
        </div>
        <div>
          <el-dropdown-menu slot="dropdown">
            <el-dropdown-item command="user">个人信息</el-dropdown-item>
            <el-dropdown-item command="logout">退出登录</el-dropdown-item>
          </el-dropdown-menu>
        </div>
      </el-dropdown>
      <div class="btn">

      </div>
    </div>
  </div>
</template>

<script>
import { get, post } from '../../plugins/request'
import NoticesComponent from "./NoticesComponent.vue";
import emitter from '../../plugins/mitt'
export default {
  name: "AppHeader",
  data() {
    return {
      //显隐弹窗
      isShow: false,
      currentNotify: "",
      userInfo: {},
    }
  },
  mounted() {
    //初始化通知
    this.initNotify();
    //初始化头像用户名信息
    this.initUserInfo();
    //这里用于暴漏出外部数据，从而当修改信息的时候，达到实时同步
    emitter.on("changeInfo", e => this.initUserInfo())

  },
  components: {
    NoticesComponent,
  },
  computed: {
    profileCommand() {
      return { action: "profile", path: "/user" };
    },
  },
  methods: {
    //初始化头像用户名
    async initUserInfo() {
      let res = await get("/getUserInfo");
      this.userInfo = { ...res };
    },
    //初始化通知
    async initNotify() {
      //获取通知
      let res = await get('/getNewNotice');
      this.currentNotify = res.content;
      // console.log(res);
      if (res.status == -2) {
        this.isShow = false;
      }
      else {
        this.isShow = true;
      }
    },
    //选择
    onSelect(command) {
      // console.log(command);
      if (command == "user") {
        this.$router.push("/user");
      }
      if (command == "logout") {
        localStorage.removeItem('token');
        this.$router.push("/login");

      }
    },
    onShutDown() {
      this.$router.push("/dashboard");
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
  font-size: 1.2rem;
  /* min-width: 50rem; */
  padding: 0;
}

.app-header .left {
  display: flex;
  flex: 1;
  /* background-color: aquamarine; */
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

.el-dropdown:hover {
  color: blue;
}

.el-header {
  width: 100vw;
}

.center {
  background-color: rgb(72, 72, 72);
  width: 100%;
  height: 100%;
  flex: 1;
}
</style>
