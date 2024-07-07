<template>
  <div class="loginContainer">
    <vue-particles
      id="tsparticles"
      :particlesInit="particlesInit"
      :particlesLoaded="particlesLoaded"
      :options="data.options"
    />
    <!-- <video
      class="video"
      src="@/assets/video/bg.mp4"
      autoplay
      loop
      muted
    ></video> -->
    <transition name="fade-down" appear>
      <div class="login">
        <el-form ref="ruleFormRef" :model="userForm" :rules="rules" status-icon>
          <el-form-item>
            <!-- <div class="header">跨时空控制台注册页面</div> -->
            <div class="header">{{ setting.title }}注册页面</div>
          </el-form-item>
          <!-- 第一次校验 -->
          <div v-if="!isOpenSecond">
            <el-form-item prop="username">
              <el-input
                :prefix-icon="User"
                v-model="userForm.username"
                placeholder="用户昵称"
              />
            </el-form-item>
            <el-form-item prop="phone">
              <el-input
                :prefix-icon="Iphone"
                v-model="userForm.phone"
                placeholder="手机号"
              />
            </el-form-item>
            <el-form-item prop="password">
              <el-input
                v-model="userForm.password"
                :prefix-icon="Key"
                show-password
                placeholder="密码"
              />
            </el-form-item>
            <el-form-item prop="repassword">
              <el-input
                v-model="userForm.repassword"
                :prefix-icon="Key"
                show-password
                placeholder="重复密码"
              />
            </el-form-item>
          </div>
          <!-- 第二次校验 -->
          <div v-else>
            <el-form-item prop="question1">
              <el-input
                :prefix-icon="InfoFilled"
                v-model="userForm2.question1"
                placeholder="问题1"
              />
            </el-form-item>
            <el-form-item prop="answer1">
              <el-input
                v-model="userForm2.answer1"
                placeholder="答案1"
                :prefix-icon="InfoFilled"
              />
            </el-form-item>
            <el-form-item prop="question2">
              <el-input
                v-model="userForm2.question2"
                placeholder="问题2"
                :prefix-icon="InfoFilled"
              />
            </el-form-item>
            <el-form-item prop="answer2">
              <el-input
                v-model="userForm2.answer2"
                :prefix-icon="InfoFilled"
                placeholder="答案2"
              />
            </el-form-item>
          </div>
          <div style="text-align: center" v-if="!isOpenSecond">
            <el-button
              style="width: 50%; border-radius: 10px"
              type="primary"
              color="#626aef"
              class="btn"
              @click="onFirstValidate"
            >
              确定
            </el-button>
          </div>
          <div style="text-align: center" v-else>
            <el-button
              style="width: 50%; border-radius: 10px"
              type="primary"
              color="#626aef"
              class="btn"
              :loading="isLoading"
              @click="toRegister"
            >
              注册
            </el-button>
          </div>

          <div style="text-align: center; margin-top: 1rem">
            <el-link type="warning" @click="toLogin">已有账号去登录？</el-link>
          </div>
        </el-form>
      </div>
    </transition>
  </div>
</template>

<script setup>
import { updateUserInfoApi } from "@/api/user/index";
import setting from "@/utils/setting";
import { loadFull } from "tsparticles";
import { useRouter } from "vue-router";
import { Key, User, InfoFilled, Iphone } from "@element-plus/icons-vue";
import { reactive, ref } from "vue";
import useUserStore from "@/store/modules/user/user";
import { ElMessage } from "element-plus";
import { SET_TOKEN } from "@/utils/token";
const userStore = useUserStore();
const ruleFormRef = ref();
const $router = useRouter();
const data = reactive({
  options: {
    fpsLimit: 100,
    interactivity: {
      events: {
        onClick: {
          enable: true,
          mode: "push",
        },
        onHover: {
          enable: true,
          mode: "grab",
        },
        resize: true,
      },
      modes: {
        bubble: {
          distance: 400,
          duration: 2,
          opacity: 0.6,
          size: 10,
        },
        push: {
          quantity: 4,
        },
        repulse: {
          distance: 200,
          duration: 0.4,
        },
      },
    },
    particles: {
      color: {
        value: "#ffffff",
      },
      links: {
        color: "#ffffff",
        distance: 150,
        enable: true,
        opacity: 0.5,
        width: 1,
      },
      collisions: {
        enable: true,
      },
      move: {
        direction: "none",
        enable: true,
        outMode: "bounce",
        random: false,
        speed: 2,
        straight: false,
      },
      number: {
        density: {
          enable: true,
          value_area: 800,
        },
        value: 60,
      },
      opacity: {
        value: 0.5,
      },
      shape: {
        type: "circle",
      },
      size: {
        random: true,
        value: 3,
      },
    },
    detectRetina: true,
  },
});
const particlesInit = async (engine) => {
  await loadFull(engine);
};
const particlesLoaded = async (container) => {
  console.log("Particles container loaded", container);
};
//第一次校验的表单内容
const userForm = reactive({
  username: "",
  phone: "",
  password: "",
  repassword: "",
});
//第二次的表单内容
const userForm2 = reactive({
  question1: "", // 问题1
  answer1: "", // 答案1
  question2: "", // 问题2
  answer2: "", // 答案2
});
//是否开启第二次校验
const isOpenSecond = ref(false);
const registerReq = reactive({
  phone: "", // 手机号
  username: "", // 用户名
  password: "", // 密码
  balance: 0.0, // 余额
  question1: "", // 问题1
  answer1: "", // 答案1
  question2: "", // 问题2
  answer2: "", // 答案2
  registerTime: new Date().toLocaleString().replaceAll("/", "-"), // 注册时间 2024-4-6 12:03:59
});
//校验手机号
const validatePhone = (rule, value, callback) => {
  let reg_tel =
    /^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/; //11位手机号码正则
  if (value == "") {
    callback(new Error("请输入你的手机号"));
  } else if (!reg_tel.test(value)) {
    callback(new Error("请正确填写您的手机号码！"));
  } else {
    callback();
  }
};
//校验密码
const validatePass = (rule, value, callback) => {
  if (value === "") {
    callback(new Error("请再次输入密码"));
  } else if (value != userForm.password) {
    callback(new Error("两次输入密码不一致!"));
  } else {
    callback();
  }
};
//设置表单校验
const rules = {
  username: [
    {
      required: true,
      message: "必须输入用户昵称",
      trigger: "change",
    },
  ],
  phone: [
    {
      required: true,
      validator: validatePhone,
      trigger: "change",
    },
  ],
  password: [
    {
      required: true,
      message: "必须输入密码！",
      trigger: "change",
    },
    {
      min: 6,
      max: 16,
      message: "密码长度在6-16位之间！",
      trigger: "change",
    },
  ],
  repassword: [
    {
      required: true,
      validator: validatePass,
      trigger: "change",
    },
    {
      min: 6,
      max: 16,
      message: "密码长度在6-16位之间",
      trigger: "change",
    },
  ],
};
//是否让注册变为loading效果
const isLoading = ref(false);
//验证第一个表单的回调
const onFirstValidate = () => {
  ruleFormRef.value.validate(async (res) => {
    //如果第一次的手机号密码都通过，那么我们就进入提出问题的环节
    if (res) {
      isOpenSecond.value = true;
      ElMessage({
        type: "info",
        message: "为了保证数据安全，请填写找回密码的相关问题",
        duration: "1000",
        showClose: true,
      });
    }
  });
};
//去注册
const toRegister = async () => {
  if (
    userForm2.question1.trim() == "" ||
    userForm2.question2.trim() == "" ||
    userForm2.answer1.trim() == "" ||
    userForm2.answer2.trim() == ""
  ) {
    ElMessage({
      type: "error",
      message: "请正确填写问题或者答案",
      duration: "2000",
      showClose: true,
    });
  } else {
    //发送请求注册用户，如果用户已被注册，就分情况
    registerReq.phone = userForm.phone;
    registerReq.password = userForm.password;
    registerReq.username = userForm.username;
    // registerReq.balance = userForm.phone;
    registerReq.question1 = userForm2.question1;
    registerReq.question2 = userForm2.question2;
    registerReq.answer1 = userForm2.answer1;
    registerReq.answer2 = userForm2.answer2;
    // console.log(registerReq);
    isLoading.value = true;
    let res = await userStore.userRegister(registerReq);
    isLoading.value = false;
    if (res.status == 0) {
      await userStore.userLogin({
        phone: registerReq.phone,
        password: registerReq.password,
      });
      ElMessage({
        type: "success",
        message: "注册成功",
        duration: "2000",
        showClose: true,
      });
      //注册成功，添加token
      SET_TOKEN(userForm.phone);
      await updateUserInfoApi({
        avatar: "/1.jpg",
      });
      console.log("%c注册成功了", "color:green");
      setTimeout(() => {
        $router.push("/home");
      }, 1500);
    } else {
      ElMessage({
        type: "error",
        message: res.message,
        duration: "2000",
        showClose: true,
      });
    }
  }
};
//去登录
const toLogin = async () => {
  $router.push("/newlogin");
};
</script>

<style scoped lang="scss">
.loginContainer {
  width: 100vw;
  height: 100vh;
  // background-color: rgb(0, 0, 0);
  background-image: url("../../assets/images/bg 30.jpg");
  display: flex;
  justify-content: center;
  align-items: center;

  .video {
    width: 100vw;
    height: 100vh;
    min-width: 100vw;
    object-fit: fill;
    position: absolute;
    top: 0;
    left: 0;
  }

  .login {
    -webkit-user-select: none; /*谷歌 /Chrome*/
    -moz-user-select: none; /*火狐/Firefox*/
    -ms-user-select: none; /*IE 10+*/
    width: 20rem;
    border: 1px solid rgb(255, 255, 255);
    border-radius: 10px;
    position: absolute;
    padding: 50px;
    user-select: none;
    .header {
      text-align: center;
      font-weight: 800;
      font-size: 1.5rem;
      margin: 0 auto;
      color: rgb(255, 255, 255);

      //动画
      background: -webkit-linear-gradient(
        135deg,
        rgb(249, 64, 17),
        #ff6ac6 25%,
        #147b96 50%,
        #e6d205 55%,
        #2cc4e0 60%,
        #ffffff 80%,
        #ff6384 95%,
        #7708df
      );
      /* 文字颜色填充设置为透明 */
      -webkit-text-fill-color: transparent;
      /* 背景裁剪，即让文字使用背景色 */
      -webkit-background-clip: text;
      /* 背景图放大一下，看着柔和一些 */
      -webkit-background-size: 200% 100%;
      /* 应用动画flowCss 12秒速度 无限循环 线性匀速动画*/
      -webkit-animation: flowCss 12s infinite linear;
      @-webkit-keyframes flowCss {
        0% {
          /* 移动背景位置 */
          background-position: 0 0;
        }

        100% {
          background-position: -400% 0;
        }
      }

      h1:hover {
        -webkit-animation: flowCss 4s infinite linear;
      }
    }
  }
  .forgetLink {
    text-align: right;
    margin-bottom: 1rem;
  }
}

/* 基础过渡设置，调整了缓动函数以获得更炫的效果 */
.fade-down-enter-active {
  animation: myTran 0.8s ease-in-out;
}

/* 定义增强的动画关键帧 */
@keyframes myTran {
  from {
    transform: translateY(-50%) rotate(-10deg) scale(0.8);
    opacity: 0;
    filter: blur(5px); /* 初始带有轻微模糊效果 */
  }

  50% {
    transform: translateY(-25%) rotate(5deg) scale(0.95);
    opacity: 0.5;
    filter: blur(0); /* 模糊消失 */
  }

  to {
    transform: translateY(0%) rotate(0deg) scale(1);
    opacity: 1;
    box-shadow:
      0 0 20px rgba(255, 255, 255, 0.5) inset,
      0 0 10px rgba(255, 255, 255, 0.3); /* 添加内阴影和外发光效果 */
  }
}
.btn {
  background: linear-gradient(-45deg, #dae, #f66, #3c9, #09f, #66f);
  background-size: 200% 200%;
  animation: gradient 8s ease infinite;
  @keyframes gradient {
    0% {
      background-position: 0 12%;
    }

    50% {
      background-position: 100% 100%;
    }

    100% {
      background-position: 0 12%;
    }
  }
}
</style>
