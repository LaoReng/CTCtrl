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
          <div class="body-top">跨时空控制台登录页面</div>
          <div class="body-main">
            <!-- 当我们想要去拿子组件或者DOM实例时，绑定ref来获取该组件实例 -->
            <el-form ref="userForm" label-width="6rem" :model="userForm" :rules="rules">
              <el-form-item label="手机号" prop="phone">
                <el-input :disabled="!interceptorsSign" v-direction="{ x: 0, y: 0 }" v-model="userForm.phone"
                  size="mini" prefix-icon="el-icon-user" placeholder="请输入手机号..." autofocus></el-input>
              </el-form-item>

              <!-- 没有点击忘记密码 -->
              <div v-show="!isForgetPassword">
                <el-form-item label="密码" prop="password">
                  <el-input :disabled="!interceptorsSign" v-direction="{ x: 0, y: 1 }" v-model="userForm.password"
                    size="mini" prefix-icon="el-icon-key" placeholder="请输入密码..." show-password></el-input>
                </el-form-item>
              </div>

              <!-- 点击获取问题 -->
              <!-- isForgetPassword=true forgetButton -->
              <div style="text-align: right;" v-if="isClickForgetButton">
                <el-button size="mini" @click="clickGetQuestion('userForm')">获取问题</el-button>
              </div>

              <div v-if="!isClickForgetButton && isForgetPassword">
                <el-form-item label="问题1" prop="input1">
                  <div class="demo-input-suffix">
                    <el-input v-model="questionObj.question1" disabled>
                    </el-input>
                    <el-input v-direction="{ x: 0, y: 2 }" placeholder="请输入内容" v-model="userForm.input1">
                    </el-input>
                  </div>
                </el-form-item>
                <el-form-item label="问题2" prop="input2">
                  <div>
                    <el-input v-model="questionObj.question2" disabled>
                    </el-input>
                    <el-input v-direction="{ x: 0, y: 3 }" placeholder="请输入内容" v-model="userForm.input2">
                    </el-input>
                  </div>
                </el-form-item>
              </div>



              <!-- 点击忘记密码 -->
              <div class="forgetPassword">
                <el-link type="primary" v-show="!isForgetPassword && interceptorsSign"
                  @click="forgetPassword">忘记密码</el-link>
              </div>

              <el-form-item :style="{ textAlign: 'center', marginLeft: '-3.5rem' }">
                <el-button v-show="!isForgetPassword" type="primary" size="mini" @click="onLogin('userForm')"
                  :disabled="!interceptorsSign">登录</el-button>
                <div v-show="isForgetPassword" class="btnGroup">
                  <el-button class="btn1" type="primary" size="mini" @click="onSubmit('userForm')">提交</el-button>
                  <el-button class="btn2" type="primary" size="mini" @click="clickCancel">取消</el-button>
                </div>
              </el-form-item>
            </el-form>
          </div>
          <div @click="toRegister" v-show="interceptorsSign" class="bottom">没有账号？去注册</div>
        </div>
      </transition>
    </div>
  </div>
</template>

<script>
import { get, post } from '../plugins/request'
import { checkMobile } from '../common/validatePhone'
export default {
  name: "Login",
  data() {
    return {
      //手机端拦截标志
      interceptorsSign: true,
      //保存当前的enter事件
      keyDownEvent: "",
      //点击获取密码按钮
      isClickForgetButton: false,
      // 点击忘记密码
      isForgetPassword: false,
      loading: false,
      questionObj: {
        question1: "",
        question2: ""
      },
      // 添加表单数据对象
      userForm: {
        phone: "",
        password: "",
        input1: "",
        input2: "",
      },
      // 设置表单的验证规则
      rules: {
        phone: [
          {
            required: true,
            validator: this.validatePhone,
            trigger: "blur",

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
        input1: [
          {
            required: true,
            message: "必须输入问题",
            trigger: "blur"
          }
        ],
        input2: [
          {
            required: true,
            message: "必须输入问题",
            trigger: "blur"
          }
        ]
      },
    };
  },
  methods: {
    //手机端登录拦截
    phoneLoginInterceptors() {


      var os = function () {
        var ua = navigator.userAgent,
          isWindowsPhone = /(?:Windows Phone)/.test(ua),
          isSymbian = /(?:SymbianOS)/.test(ua) || isWindowsPhone,
          isAndroid = /(?:Android)/.test(ua),
          isFireFox = /(?:Firefox)/.test(ua),
          isChrome = /(?:Chrome|CriOS)/.test(ua),
          isTablet = /(?:iPad|PlayBook)/.test(ua) || (isAndroid && !/(?:Mobile)/.test(ua)) || (isFireFox && /(?:Tablet)/.test(ua)),
          isPhone = /(?:iPhone)/.test(ua) && !isTablet,
          isPc = !isPhone && !isAndroid && !isSymbian;
        return {
          isTablet: isTablet,
          isPhone: isPhone,
          isAndroid: isAndroid,
          isPc: isPc
        };
      }();

      if (os.isAndroid || os.isPhone) {
        console.log("手机")
        this.interceptorsSign = false;
        alert("当前手机端页面暂无适配\n请在电脑端访问网站")

      } else if (os.isTablet) {
        console.log("平板")
      } else if (os.isPc) {
        console.log("电脑")
      }


    },
    async onSubmit(formName) {
      if (this.userForm.input1.trim() == "" || this.userForm.input2.trim() == "") {
        this.$message({
          message: "请输入问题",
          type: "error",
          duration: 1000,
        })
      }
      else {
        let res = await post('/verifyAnswer', {
          phone: this.userForm.phone,
          answer1: this.userForm.input1,
          answer2: this.userForm.input2
        })
        // console.log(res);
        if (res.status == 1) {
          this.userForm.password = res.password;
          this.clickCancel();
        }
        else {
          this.$message({
            message: "问题不正确",
            type: "error",
            duration: 1000,
          })
        }
      }
    },
    //点击获取问题
    async clickGetQuestion(formName) {
      if (checkMobile(this.userForm.phone)) {

        //发起忘记密码的请求
        let res = await post("/forgetPassword", {
          phone: this.userForm.phone
        })

        if (res.status == 1) {
          this.isClickForgetButton = false;
          this.isForgetPassword = true;

          this.questionObj.question1 = res.question1;
          this.questionObj.question2 = res.question2;
        }
        //未注册
        else {
          this.$message({
            message: '当前账号未被注册,即将在2s后跳转到注册页面',
            type: 'error'
          });
          setTimeout(() => {
            this.$router.push("/register")
          }, 2000)
        }
        // console.log(res);

      }
      else {

      }
    },
    //取消
    clickCancel() {
      this.isForgetPassword = false;
      this.isClickForgetButton = false;
    },
    //检验手机号
    validatePhone(rule, value, callback) {
      var reg_tel =
        /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/; //11位手机号码正则
      if (value == "") {
        callback(new Error("请输入你的手机号"));
      } else if (!reg_tel.test(value)) {
        callback(new Error("请正确填写您的手机号码！"));
      }
      else {
        callback()
      }
    },
    //忘记密码
    forgetPassword() {
      this.isClickForgetButton = true;
      this.isForgetPassword = true;

      //清空表单的密码
      this.userForm.password = ""

    },
    //点击登录
    async onLogin(formName) {
      if (this.interceptorsSign == false) {
        alert("当前暂不支持手机端，请用网页端访问");
        return;
      }
      //如果没有点击忘记密码
      if (this.isForgetPassword == false) {
        this.$refs[formName].validate(async (item) => {
          if (item) {
            try {
              let res = await get("/login", this.userForm)
              if (res.status == 0) {
                localStorage.setItem("token", this.userForm.phone)
                console.log(this);
                this.$message({
                  // 可关闭
                  showClose: true,
                  message: "登录成功",
                  type: "success",
                });
                this.$router.push("/dashboard");
              }
              else {
                this.$message({
                  // 可关闭
                  showClose: true,
                  message: "用户名或者密码错误",
                  type: "error",
                });
              }
            } catch (error) {
              this.$message({
                // 可关闭
                showClose: true,
                message: "服务器不在线",
                type: "error",
              });
            }
          }
        })

      }
    },
    toRegister() {
      this.$router.push("/register");
    },
  },
  async mounted() {
    this.phoneLoginInterceptors();
    const event = (e) => {
      //如果点击的是enter键。那么就相当于点击了登录按钮
      if (e.keyCode == 13) {
        this.onLogin('userForm')
      }
    }
    window.addEventListener('keydown', event)
    this.keyDownEvent = event;
  },
  created: function () {
    let direction = this.$getDirection()
    direction.on('keydown', function (e, val) {
      // console.log(val);
      // console.log(direction.next());
      //上
      if (e.keyCode === 38) {
        direction.previousLine()
      }
      //下
      if (e.keyCode === 40) {
        direction.nextLine()
      }
    })
  },
  beforeDestroy() {
    if (this.keyDownEvent) {
      window.removeEventListener('keydown', this.keyDownEvent);
      this.keyDownEvent = null;
    }

  }
};
</script>

<style lang="css" scoped>
.btnGroup {
  margin-top: 5%;
  display: flex;
  /* background-color: red; */
  justify-content: space-around;
}



.containerLogin {
  min-width: 50%;
  width: 100%;
}

.demo-input-suffix>>>.el-input__inner {
  color: black;
}

.el-input>>>.el-input__inner {
  color: black;
}


.forgetPassword {
  text-align: right;
}

.login {
  background: url(../assets/bg.jpg);
  background-size: 100vw 100vh;
  /* 图片 */
  background-repeat: repeat;
  width: 100%;
  height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
}

.login .login-body {
  width: 23.5rem;
  /* height: 24rem; */
  background-color: #ccc;
  border-radius: 0.7rem;
  padding-top: 1.42rem;
}

.login .login-body .body-top {
  color: #333;
  font-size: 1.7rem;
  font-weight: 700;
  text-align: center;
  margin-bottom: 1.42rem;
}

.login .login-body .body-main {
  padding-right: 2.57rem;
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
