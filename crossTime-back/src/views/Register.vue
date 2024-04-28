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
            <el-form
              ref="userForm"
              label-width="100px"
              :model="userForm"
              :rules="rules"
              v-if="isShow"
            >
              <div>
                <el-form-item label="用户名" prop="username">
                  <el-input
                    v-model="userForm.username"
                    size="mini"
                    prefix-icon="el-icon-user"
                    placeholder="请输入用户名..."
                    autofocus
                  ></el-input>
                </el-form-item>
                <el-form-item label="手机号" prop="phone">
                  <el-input
                    v-model="userForm.phone"
                    size="mini"
                    prefix-icon="el-icon-user"
                    placeholder="请输入手机号..."
                    autofocus
                  ></el-input>
                </el-form-item>

                <el-form-item label="密码" prop="password">
                  <el-input
                    v-model="userForm.password"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请输入密码..."
                    show-password
                  ></el-input>
                </el-form-item>

                <el-form-item label="重复密码" prop="repassword">
                  <el-input
                    v-model="userForm.repassword"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请再次输入相同密码..."
                    show-password
                  ></el-input>
                </el-form-item>
              </div>

              <el-form-item :style="{ textAlign: 'right' }">
                <el-button
                  type="primary"
                  round
                  size="mini"
                  @click="onRegister1('userForm')"
                  >注册</el-button
                >
              </el-form-item>
            </el-form>

            <el-form
              ref="question"
              label-width="100px"
              :model="question"
              :rules="rules"
              v-else
            >
              <div>
                <el-form-item label="问题1" prop="question1">
                  <el-input
                    v-model="question.question1"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请输入第一个问题"
                    show-password
                  ></el-input>
                </el-form-item>
                <el-form-item label="答案1" prop="answer1">
                  <el-input
                    v-model="question.answer1"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请输入第一个问题答案..."
                    show-password
                  ></el-input>
                </el-form-item>

                <el-form-item label="问题2" prop="question2">
                  <el-input
                    v-model="question.question2"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请输入第二个问题"
                    show-password
                  ></el-input>
                </el-form-item>
                <el-form-item label="答案2" prop="answer2">
                  <el-input
                    v-model="question.answer2"
                    size="mini"
                    prefix-icon="el-icon-key"
                    placeholder="请输入第二个问题答案..."
                    show-password
                  ></el-input>
                </el-form-item>
              </div>

              <el-form-item :style="{ textAlign: 'right' }">
                <el-button
                  type="primary"
                  round
                  size="mini"
                  @click="onRegister2('question')"
                  >注册</el-button
                >
              </el-form-item>
            </el-form>
          </div>
          <div @click="toLogin" class="bottom">已有账号？去登录</div>
        </div>
      </transition>
    </div>
  </div>
</template>

<script>
import { post } from "../plugins/request";
// import axios from 'axios'
export default {
  name: "Login",
  data() {
    return {
      isShow: true,

      // 添加表单数据对象
      userForm: {
        // 下面的属性名称尽量和后台接口对应上
        phone: "",
        username: "",
        password: "",
        repassword: "",
      },
      question: {
        question1: "",
        answer1: "",
        question2: "",
        answer2: "",
      },
      totalForm: {
        phone: "",
        username: "",
        password: "",
        question1: "",
        answer1: "",
        question2: "",
        answer2: "",
        balance: 0,
        registerTime: new Date().toLocaleString().replaceAll("/", "-"),
      },
      // 设置表单的验证规则
      rules: {
        username: [
          {
            required: true,
            message: "必须输入账户",
            trigger: "blur",
          },
        ],
        password: [
          { required: true, validator: this.validatePass, trigger: "blur" },
          {
            min: 6,
            max: 16,
            message: "密码长度在6-16位之间",
            trigger: "blur",
          },
        ],
        phone: [
          {
            required: true,
            validator: this.validatePhone,
            trigger: "blur",
          },
          {
            min: 11,
            max: 11,
            message: "手机号必须11位",
            trigger: "blur",
          },
        ],
        repassword: [
          {
            required: true,
            validator: this.validatePass2,
            trigger: "blur",
          },
          {
            min: 6,
            max: 16,
            message: "密码长度在6-16位之间",
            trigger: "blur",
          },
        ],

        question1: [
          {
            required: true,
            message: "必须输入问题1",
            trigger: "blur",
          },
        ],
        answer1: [
          {
            required: true,
            validator: this.validateQuestion,
            trigger: "blur",
          },
        ],
        question2: [
          {
            required: true,
            message: "必须输入问题2",
            trigger: "blur",
          },
        ],
        answer2: [
          {
            required: true,
            message: "必须输入答案2",
            trigger: "blur",
          },
        ],
      },
    };
  },
  methods: {
    //验证第一个密码
    validatePass(rule, value, callback) {
      //   console.log(this.$refs);
      //   console.log(value, callback());
      if (value == "") {
        callback(new Error("请输入密码"));
      } else {
        if (this.userForm.repassword !== "") {
          this.$refs.userForm.validateField("repassword");
        }
        callback();
      }
    },
    //验证第二个密码
    validatePass2(rule, value, callback) {
      if (value === "") {
        callback(new Error("请再次输入密码"));
      } else if (value != this.userForm.password) {
        callback(new Error("两次输入密码不一致!"));
      } else {
        callback();
      }
    },

    //验证手机号
    validatePhone(rule, value, callback) {
      var reg_tel =
        /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/; //11位手机号码正则
      if (value == "") {
        callback(new Error("请输入你的手机号"));
      } else if (!reg_tel.test(value)) {
        callback(new Error("请正确填写您的手机号码！"));
      } else {
        callback();
      }
    },
    async onRegister1(formName) {
      this.$refs[formName].validate((valid) => {
        if (valid) {
          this.isShow = false;
          this.$notify({
            title: "成功",
            message: "为了保证你的信息安全，请自定义验证信息",
            type: "success",
          });
          return true;
        } else {
          console.log("error submit!!");
          return false;
        }
      });
    },
    //第二个表单
    async onRegister2(formName) {
      this.$refs[formName].validate(async (valid) => {
        if (valid) {
          console.log("成功");
          console.log(this.userForm, this.question);
          this.totalForm.phone = this.userForm.phone;
          this.totalForm.username = this.userForm.username;
          this.totalForm.password = this.userForm.password;
          this.totalForm.question1 = this.question.question1;
          this.totalForm.answer1 = this.question.answer1;
          this.totalForm.question2 = this.question.question2;
          this.totalForm.answer2 = this.question.answer2;

          let res = await post("/register", this.totalForm);
          console.log(res);
          if (res.status == 0) {
            this.$message({
              // 可关闭
              showClose: true,
              message: "注册成功，即将跳转到登录页面...",
              type: "success",
            });
            setTimeout(() => {
              this.$router.push("/login");
            }, 2000);
          } else {
            this.$message({
              // 可关闭
              showClose: true,
              message: "手机号已被注册",
              type: "error",
            });
          }
          return true;
        } else {
          console.log("error submit!!");
          return false;
        }
      });
    },
    toLogin() {
      this.$router.push("/login");
    },
    mounted() {
      // console.log(1);
    },
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
  height: auto;
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
