<template>
  <div class="containerLogin">
    <div class="login">
      <!-- <video class="video-background" autoplay loop muted> -->
      <!-- 您的视频源 -->
      <!-- <source src="https://cdn.pixabay.com/video/2020/02/24/32767-394004551_large.mp4" type="video/mp4"> -->
      <!-- 如果需要，您可以提供其他视频格式的源 -->
      <!-- </video> -->
      <!-- 必须加上appear初次渲染才能显示 -->
      <transition name="fade-down" appear>
        <div class="login-body">
          <div class="body-top">跨时空控制台后台管理系统</div>
          <div class="body-main">
            <!-- 当我们想要去拿子组件或者DOM实例时，绑定ref来获取该组件实例 -->
            <el-form ref="userForm" label-width="100px" :model="userForm" :rules="rules">
              <el-form-item label="手机号" prop="phone">
                <el-input v-model="userForm.phone" size="mini" prefix-icon="el-icon-user" placeholder="请输入账号..."
                  autofocus></el-input>
              </el-form-item>

              <el-form-item label="密码" prop="password">
                <el-input v-model="userForm.password" size="mini" prefix-icon="el-icon-key" placeholder="请输入密码..."
                  show-password></el-input>
              </el-form-item>

              <el-form-item :style="{ textAlign: 'right' }">
                <el-button type="primary" round size="mini" @click="onLogin('userForm')">登录</el-button>
              </el-form-item>
            </el-form>
          </div>
          <div @click="toRegister" class="bottom">没有账号？去注册</div>
        </div>
      </transition>
    </div>
  </div>
</template>

<script>
import { get } from "../plugins/request";
export default {
  name: "Login",
  data() {
    return {
      loading: false,
      // 添加表单数据对象
      userForm: {
        // 下面的属性名称尽量和后台接口对应上
        phone: "",
        password: "",
      },
      // 设置表单的验证规则
      rules: {
        phone: [
          {
            required: true,
            message: "必须输入手机号！",
            trigger: "blur",
            // 这个blur是
          },
        ],
        password: [
          {
            required: true,
            message: "必须输入密码！",
            trigger: "blur",
          },
          {
            min: 3,
            max: 16,
            message: "密码长度在3-16位之间！",
            trigger: "blur",
          },
        ],
      },
    };
  },
  methods: {
    async onLogin(formName) {
      let valid = this.$refs[formName].validate(async (item) => {
        if (item) {
          let res = await get("/login", this.userForm);
          // console.log(res);
          if (res.status == 0) {
            localStorage.setItem("token", this.userForm.phone);
            this.$message({
              // 可关闭
              showClose: true,
              message: "登录成功",
              type: "success",
            });
            this.$router.push("/user");
          } else {
            this.$message({
              // 可关闭
              showClose: true,
              message: "用户名或者密码错误",
              type: "error",
            });
          }
        }
      });
    },
    toRegister() {
      this.$router.push("/register");
    },
  },
  mounted() {
    // // console.log(new Date().toLocaleString().replaceAll("/","-"));
    // console.log(process.env);
  },
};
</script>

<style lang="css" scoped>
.login {
  background: url(../assets/bg.jpg);
  background-size: 100vw 100vh;
  /* 图片 */
  background-repeat: no-repeat;
  width: 100%;
  height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
}

.login .login-body {
  width: 400px;
  height: 265px;
  background-color: #ccc;
  border-radius: 10px;
  padding-top: 20px;
}

.login .login-body .body-top {
  color: #333;
  font-size: 24px;
  font-weight: 700;
  text-align: center;
  margin-bottom: 20px;
}

.login .login-body .body-main {
  padding-right: 36px;
}

.fade-down-enter-active {
  animation: mytran 0.7s linear;
}

@keyframes mytran {
  from {
    transform: translateY(-50%);
    opacity: 0;
  }

  to {
    transform: translateY(0%);
    opacity: 1;
  }
}

.bottom {
  text-align: center;
  color: blue;
  text-decoration: underline;
}

.bottom:hover {
  color: red;
  cursor: pointer;
}
</style>
