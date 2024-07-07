<template>
  <div class="loginContainer">
    <vue-particles
      id="tsparticles"
      :particlesInit="particlesInit"
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
        <el-form
          ref="ruleFormRef"
          :model="userForm"
          :rules="rules"
          status-icon
          style="height: 100%"
        >
          <!-- 标题 -->
          <el-form-item>
            <!-- <div class="header">跨时空控制台登录页面</div> -->
            <div class="header">{{ setting.title }}登录页面</div>
          </el-form-item>
          <!-- 用户名 -->
          <el-form-item prop="phone">
            <el-input
              :prefix-icon="User"
              v-model="userForm.phone"
              placeholder="手机号/用户名"
            />
          </el-form-item>
          <!-- 密码 -->
          <el-form-item v-show="!isClickPassword" prop="password">
            <el-input
              v-model="userForm.password"
              :prefix-icon="Key"
              show-password
              placeholder="密码"
            />
          </el-form-item>
          <!-- 问题,在点击获取问题按钮成功的时候显示 -->
          <div v-show="isShowQuestionForm">
            <el-form-item style="width: 100%">
              <el-input
                v-model="questionObj.question1"
                disabled
                placeholder="问题1"
              />
            </el-form-item>
            <el-form-item style="width: 100%">
              <el-input v-model="questionObj.answer1" placeholder="答案" />
            </el-form-item>
            <el-form-item style="width: 100%">
              <el-input
                v-model="questionObj.question2"
                disabled
                placeholder="问题2"
              />
            </el-form-item>
            <el-form-item style="width: 100%">
              <el-input v-model="questionObj.answer2" placeholder="答案" />
            </el-form-item>
          </div>
          <!-- 获取问题 -->
          <el-form-item v-show="isClickPassword == true">
            <div class="getQuestion">
              <el-button
                type="primary"
                v-show="isClickPassword"
                @click="ClickBack"
                size="small"
              >
                返回
              </el-button>
              <el-button
                v-show="!isShowQuestionForm"
                @click="getQuestionCallback"
                type="primary"
                size="small"
              >
                获取问题
              </el-button>
            </div>
          </el-form-item>
          <!-- 忘记密码 -->
          <div class="forgetLink" v-show="!isClickPassword">
            <el-link type="warning" @click="forgetPassword">忘记密码</el-link>
          </div>
          <!-- 登录 -->
          <div style="text-align: center" v-if="!isShowQuestionForm">
            <el-button
              v-show="!isClickPassword"
              style="width: 50%; border-radius: 10px"
              type="primary"
              class="btn"
              color="#626aef"
              @click="onLogin"
              :disabled="limitPhone"
            >
              登录
            </el-button>
            <div class="noAccount">
              <el-link type="warning" @click="toRegister">
                没有账号去注册?
              </el-link>
            </div>
          </div>
          <!-- 提交答案 -->
          <div style="text-align: center" v-else>
            <el-button
              style="width: 50%; border-radius: 10px"
              type="primary"
              color="#626aef"
              @click="onSubmit"
            >
              提交
            </el-button>
          </div>
        </el-form>
      </div>
    </transition>
    <!-- 下载中心 -->
    <el-popover
      placement="top-start"
      :popper-style="{
        userSelect: 'none',
      }"
      trigger="hover"
    >
      <template #reference>
        <div
          @click="toDownload"
          class="img"
          @mouseenter="isPointer = true"
          @mouseleave="isPointer = false"
        >
          <img v-show="isPointer" src="@/assets/download.png" />
          <img v-show="!isPointer" src="@/assets/download_active.png" />
        </div>
      </template>
      <div>点击进入下载中心</div>
    </el-popover>
    <el-popover
      placement="top-start"
      :popper-style="{
        userSelect: 'none',
      }"
      trigger="hover"
    >
      <template #reference>
        <div
          @click="toIntroduce"
          class="introduce"
          @mouseenter="isIntroduce = true"
          @mouseleave="isIntroduce = false"
        >
          <!-- <img v-show="isIntroduce" src="@/assets/12.png" /> -->

          <svg
            v-show="!isIntroduce"
            t="1719133970627"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="9801"
            width="48"
            height="48"
          >
            <path
              d="M803.84 220.16C727.04 143.36 619.52 102.4 512 102.4c-225.28 0-409.6 184.32-409.6 409.6s184.32 409.6 409.6 409.6c107.52 0 215.04-40.96 291.84-117.76 76.8-76.8 117.76-179.2 117.76-291.84 0-107.52-40.96-215.04-117.76-291.84z m-271.36 517.12c0 15.36-10.24 25.6-25.6 25.6h-25.6c-15.36 0-25.6-10.24-25.6-25.6v-30.72c0-15.36 10.24-25.6 25.6-25.6h25.6c15.36 0 25.6 10.24 25.6 25.6v30.72z m20.48-220.16c-5.12 5.12-5.12 5.12 0 0-20.48 30.72-30.72 56.32-35.84 87.04 0 10.24-10.24 20.48-25.6 20.48s-25.6-10.24-25.6-25.6c5.12-56.32 35.84-112.64 87.04-163.84 0 0 0-5.12 5.12-5.12 15.36-20.48 20.48-40.96 15.36-66.56V358.4c-10.24-35.84-30.72-51.2-61.44-51.2h-10.24c-30.72 0-51.2 20.48-56.32 61.44 0 10.24-10.24 20.48-25.6 20.48s-25.6-10.24-25.6-25.6c5.12-51.2 25.6-81.92 66.56-97.28h5.12c10.24-5.12 20.48-10.24 40.96-10.24 46.08-5.12 81.92 20.48 112.64 71.68v5.12c20.48 66.56-5.12 122.88-66.56 184.32z"
              fill="rgb(191,191,191)"
              p-id="9802"
            ></path>
          </svg>
          <!-- <img v-show="!isIntroduce" src="@/assets/1.png" />
            -->
          <svg
            v-show="isIntroduce"
            t="1719133970627"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="9801"
            width="48"
            height="48"
          >
            <path
              d="M803.84 220.16C727.04 143.36 619.52 102.4 512 102.4c-225.28 0-409.6 184.32-409.6 409.6s184.32 409.6 409.6 409.6c107.52 0 215.04-40.96 291.84-117.76 76.8-76.8 117.76-179.2 117.76-291.84 0-107.52-40.96-215.04-117.76-291.84z m-271.36 517.12c0 15.36-10.24 25.6-25.6 25.6h-25.6c-15.36 0-25.6-10.24-25.6-25.6v-30.72c0-15.36 10.24-25.6 25.6-25.6h25.6c15.36 0 25.6 10.24 25.6 25.6v30.72z m20.48-220.16c-5.12 5.12-5.12 5.12 0 0-20.48 30.72-30.72 56.32-35.84 87.04 0 10.24-10.24 20.48-25.6 20.48s-25.6-10.24-25.6-25.6c5.12-56.32 35.84-112.64 87.04-163.84 0 0 0-5.12 5.12-5.12 15.36-20.48 20.48-40.96 15.36-66.56V358.4c-10.24-35.84-30.72-51.2-61.44-51.2h-10.24c-30.72 0-51.2 20.48-56.32 61.44 0 10.24-10.24 20.48-25.6 20.48s-25.6-10.24-25.6-25.6c5.12-51.2 25.6-81.92 66.56-97.28h5.12c10.24-5.12 20.48-10.24 40.96-10.24 46.08-5.12 81.92 20.48 112.64 71.68v5.12c20.48 66.56-5.12 122.88-66.56 184.32z"
              fill="#fff"
              p-id="9802"
            ></path>
          </svg>
        </div>
      </template>
      <div>点击进入使用说明</div>
    </el-popover>
  </div>
</template>

<script setup>
import setting from "@/utils/setting";
import { loadFull } from "tsparticles";
import { checkMobile } from "@/utils/validatePhone";
import { Key, User } from "@element-plus/icons-vue";
import { reactive, ref, onMounted } from "vue";
import { useRouter } from "vue-router";
import useUserStore from "@/store/modules/user/user";
import { ElMessage } from "element-plus";
const userStore = useUserStore();
const $router = useRouter();
const ruleFormRef = ref();
let limitPhone = ref(false);
const data = reactive({
  options: {
    particles: {
      number: {
        value: 160, //数量
        density: {
          enable: true, //启用粒子的稀密程度
          value_area: 800, //区域散布密度大小
        },
      },
      color: {
        value: "#ffffff", //原子的颜色
      },
      shape: {
        type: "circle", //原子的形状 "circle" ,"edge" ,"triangle" ,"polygon" ,"star" ,"image" ,["circle", "triangle", "image"]
        stroke: {
          width: 0, //原子的宽度
          color: "#000000", //原子颜色
        },
        polygon: {
          nb_sides: 5, // 原子的多边形边数
        },
        image: {
          src: "img/github.svg", // 原子的图片可以使用自定义图片 "assets/img/yop.svg" , "http://mywebsite.com/assets/img/yop.png"
          width: 100, //图片宽度
          height: 100, //图片高度
        },
      },
      opacity: {
        value: 1, //不透明度
        random: true, //随机不透明度
        anim: {
          enable: true, //渐变动画
          speed: 1, // 渐变动画速度
          opacity_min: 0, //渐变动画不透明度
          sync: true,
        },
      },
      size: {
        value: 3, //原子大小
        random: true, // 原子大小随机
        anim: {
          enable: false, // 原子渐变
          speed: 4, //原子渐变速度
          size_min: 0.3,
          sync: false,
        },
      },
      line_linked: {
        enable: false, //连接线
        distance: 150, //连接线距离
        color: "#ffffff", //连接线颜色
        opacity: 0.4, //连接线不透明度
        width: 1, //连接线的宽度
      },
      move: {
        enable: true, //原子移动
        speed: 1, //原子移动速度
        direction: "none", //原子移动方向   "none" ,"top" ,"top-right" ,"right" ,"bottom-right" ,"bottom" ,"bottom-left" ,"left" ,"top-left"
        random: true, //移动随机方向
        straight: false, //直接移动
        out_mode: "out", //是否移动出画布
        bounce: false, //是否跳动移动
        attract: {
          enable: false, // 原子之间吸引
          rotateX: 600, //原子之间吸引X水平距离
          rotateY: 600, //原子之间吸引Y水平距离
        },
      },
    },
    interactivity: {
      detect_on: "canvas", //原子之间互动检测 "canvas", "window"
      events: {
        onhover: {
          enable: true, //悬停
          mode: "bubble", //悬停模式      "grab"抓取临近的,"bubble"泡沫球效果,"repulse"击退效果,["grab", "bubble"]
        },
        onclick: {
          enable: false, //点击效果
          mode: "repulse", //点击效果模式   "push" ,"remove" ,"bubble" ,"repulse" ,["push", "repulse"]
        },
        resize: true, // 互动事件调整
      },
      modes: {
        grab: {
          distance: 100, //原子互动抓取距离
          line_linked: {
            opacity: 0.8, //原子互动抓取距离连线不透明度
          },
        },
        bubble: {
          distance: 250, //原子抓取泡沫效果之间的距离
          size: 4, // 原子抓取泡沫效果之间的大小
          duration: 2, //原子抓取泡沫效果之间的持续事件
          opacity: 1, //原子抓取泡沫效果透明度
          speed: 3,
        },
        repulse: {
          distance: 400, //击退效果距离
          duration: 0.4, //击退效果持续事件
        },
        push: {
          particles_nb: 4, //粒子推出的数量
        },
        remove: {
          particles_nb: 2,
        },
      },
    },
    retina_detect: true,
  },
});
(() => {
  let os = (function () {
    let ua = navigator.userAgent,
      isWindowsPhone = /(?:Windows Phone)/.test(ua),
      isSymbian = /(?:SymbianOS)/.test(ua) || isWindowsPhone,
      isAndroid = /(?:Android)/.test(ua),
      isFireFox = /(?:Firefox)/.test(ua),
      isTablet =
        /(?:iPad|PlayBook)/.test(ua) ||
        (isAndroid && !/(?:Mobile)/.test(ua)) ||
        (isFireFox && /(?:Tablet)/.test(ua)),
      isPhone = /(?:iPhone)/.test(ua) && !isTablet,
      isPc = !isPhone && !isAndroid && !isSymbian;
    return {
      isTablet: isTablet,
      isPhone: isPhone,
      isAndroid: isAndroid,
      isPc: isPc,
    };
  })();

  if (os.isAndroid || os.isPhone) {
    console.log("手机");
    alert("当前手机端页面暂无适配\n请在电脑端访问网站");
    limitPhone.value = true;
  } else if (os.isTablet) {
    console.log("平板");
  } else if (os.isPc) {
    console.log("电脑");
  }
})();
const particlesInit = async (engine) => {
  // console.log(engine);
  await loadFull(engine);
};
const toIntroduce = () => {
  $router.push("/desc");
};
//是否激活下载中心
let isPointer = ref(false);
let isIntroduce = ref(false);
const userForm = reactive({
  phone: "",
  password: "",
  ip: "",
});
const questionObj = reactive({
  question1: "",
  answer1: "",
  question2: "",
  answer2: "",
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
//设置表单校验
const rules = {
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
      min: 3,
      max: 16,
      message: "密码长度在3-16位之间！",
      trigger: "change",
    },
  ],
};
//是否点击忘记密码
let isClickPassword = ref(false);
//是否显示问题
let isShowQuestionForm = ref(false);
//点击返回
let ClickBack = () => {
  isClickPassword.value = false;
  isShowQuestionForm.value = false;
};
//点击忘记密码
const forgetPassword = () => {
  isClickPassword.value = true;
};
//点击了忘记密码，并且点击了获取问题
const getQuestionCallback = () => {
  ruleFormRef.value.validate(async () => {
    //如果手机号校验正确
    if (checkMobile(userForm.phone)) {
      //让当前表单的问题显示出来，发起校验问题请求
      let res = await userStore.forgetPassword({ phone: userForm.phone });
      /*
        {message: '获取成功', question1: '123', question2: '123', status: 1}
        */
      if (res.status == 1) {
        isShowQuestionForm.value = true;
        ElMessage({
          type: "success",
          message: "问题获取成功，请按照要求填写",
          duration: "1000",
          showClose: true,
        });
        questionObj.question1 = res.question1;
        questionObj.question2 = res.question2;
      } else {
        ElMessage({
          type: "info",
          message: res.message,
          duration: "1000",
          showClose: true,
        });
      }
    }
  });
};
//登录
// let value = ref(true);

const onLogin = () => {
  ruleFormRef.value.validate(async (val) => {
    //if phone and password all
    if (val) {
      let res = await userStore.userLogin(userForm);
      // console.log(res);
      if (res.status == 0) {
        //获取数据
        userStore.getUserInfo();
        ElMessage({
          type: "success",
          message: "登录成功",
        });
        $router.replace("/home");
      } else {
        ElMessage({
          type: "error",
          message: "用户名或密码错误",
        });
      }
    }
  });
};

window.addEventListener("keydown", (ev) => {
  // console.log(ev);
  ev.keyCode == 13 ? onLogin() : "";
});
//提交问题
const onSubmit = async () => {
  if (
    questionObj.question1.trim() == "" ||
    questionObj.question2.trim() == ""
  ) {
    ElMessage({
      type: "error",
      message: "内容不为空",
    });
    return;
  }
  const res = await userStore.verifyAnswer({
    phone: userForm.phone,
    answer1: questionObj.answer1,
    answer2: questionObj.answer2,
  });
  if (res.status == 1) {
    ElMessage({
      type: "success",
      message: "验证成功",
    });
    userForm.password = res.password;
    isClickPassword.value = false;
    isShowQuestionForm.value = false;
  } else {
    ElMessage({
      type: "error",
      message: "答案不正确",
    });
  }
};
//去注册
const toRegister = () => {
  $router.push("/newregister");
};
//去下载中心
const toDownload = () => {
  let routeData = $router.resolve({
    name: "download",
  });
  window.open(routeData.href, "_blank");
};

onMounted(async () => {
  await userStore.getLoginIp();
  userForm.ip = userStore.ip;
  console.log("初始化获取ip", userStore.ip);
});
</script>

<style scoped lang="scss">
.loginContainer {
  width: 100vw;
  height: 100vh;
  // background-color: #ccc;
  background-image: url("../../assets/images/bg 30.jpg");
  background-repeat: no-repeat;
  background-size: cover;
  display: flex;
  justify-content: center;
  align-items: center;
  .img {
    position: absolute;
    bottom: 10px;
    right: 20px;
    cursor: pointer;
  }
  .introduce {
    position: absolute;
    bottom: 10px;
    right: 70px;
    cursor: pointer;
  }

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
    -webkit-user-select: none;
    /*谷歌 /Chrome*/
    -moz-user-select: none;
    /*火狐/Firefox*/
    -ms-user-select: none;
    /*IE 10+*/
    user-select: none;
    width: 20rem;
    border: 1px solid rgb(255, 255, 255);
    border-radius: 10px;
    position: absolute;
    padding: 50px;
    .getQuestion {
      width: 100%;
      text-align: right;
    }

    .header {
      text-align: center;
      font-weight: 800;
      font-size: 1.5rem;
      margin: 0 auto;
      color: rgb(255, 255, 255);
      // opacity: 0.8;

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
    }
    .header:hover {
      -webkit-animation: flowCss 4s infinite linear;
    }
  }

  .forgetLink {
    text-align: right;
    margin-bottom: 1rem;
  }

  .noAccount {
    text-align: center;
    margin-top: 1rem;
    text-decoration: underline;
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
