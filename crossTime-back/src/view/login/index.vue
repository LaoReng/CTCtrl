<template>
  <div class="loginContainer">
    <!-- <video
      class="video"
      src="@/assets/video/地球.mp4"
      autoplay
      loop
      muted
      playsinline
    ></video> -->
    <vue-particles
      id="tsparticles"
      :particlesInit="particlesInit"
      :particlesLoaded="particlesLoaded"
      :options="data.options"
    />
    <transition name="fade-down" appear>
      <div class="login">
        <el-form
          ref="ruleFormRef"
          :model="form"
          :rules="rules"
          status-icon
          style="height: 100%"
        >
          <el-form-item>
            <div class="header">跨时空控制台后台管理系统</div>
          </el-form-item>
          <el-form-item prop="phone">
            <el-input
              :prefix-icon="User"
              v-model="form.phone"
              placeholder="请输入手机号"
              v-on:keyup.enter="hcLogin"
              @keydown.up.prevent="focusNextInput"
              @keydown.down.prevent="focusNextInput"
              ref="usernameInput"
            />
          </el-form-item>
          <el-form-item prop="password">
            <el-input
              v-model="form.password"
              :prefix-icon="Lock"
              show-password
              placeholder="请输入3-16位的密码"
              v-on:keyup.enter="hcLogin"
              @keydown.up.prevent="focusPreviousInput"
              @keydown.down.prevent="focusPreviousInput"
              ref="passwordInput"
            />
          </el-form-item>
          <el-button class="btn" type="primary" @click="handleLogin">
            登录
          </el-button>
        </el-form>
      </div>
    </transition>
  </div>
</template>

<script setup>
import { reactive, ref, onMounted } from "vue";
import { useAuthStore } from "@/store/auth";
import { ElMessage } from "element-plus";
import { useRoute, useRouter } from "vue-router";
import { loadFull } from "tsparticles";
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
  // console.log(engine);
  await loadFull(engine);
};
const particlesLoaded = async (container) => {
  console.log("Particles container loaded", container);
};

const authStore = useAuthStore();
const router = useRouter(); // 获取路由实例
const ruleFormRef = ref();
const form = reactive({
  phone: "",
  password: "",
});
const usernameInput = ref(null);
const passwordInput = ref(null);
onMounted(() => {
  // 初始化聚焦第一个输入框
  usernameInput.value.focus();
});
const rules = {
  phone: [
    {
      required: true,
      message: "请输入手机号",
      trigger: "blur",
    },
    {
      pattern: /^1[3-9]\d{9}$/, // 这里是匹配中国手机号的正则表达式
      message: "请输入正确的11位手机号码",
      trigger: "blur",
    },
  ],
  password: [
    {
      required: true,
      message: "请输入密码",
      trigger: "blur",
    },
    {
      min: 3,
      max: 16,
      message: "密码长度在3-16位之间",
      trigger: "blur",
    },
  ],
};
async function handleLogin() {
  // console.log(111);
  authStore.userForm.phone = form.phone;
  authStore.userForm.password = form.password;

  // console.log(authStore.userForm);
  // console.log(form.phone,form.password)
  try {
    await authStore.login(form.phone, form.password);
    if (authStore.token) {
      ElMessage({
        type: "success",
        message: "登录成功",
      });
      router.push("/users");
    }
  } catch (error) {
    ElMessage({
      type: "error",
      message: "登录失败，请检查手机号和密码",
    });
  }
}
async function hcLogin() {
  // console.log(form.phone, form.password);
  await handleLogin();
}
function focusNextInput(event) {
  passwordInput.value.focus();
}

function focusPreviousInput(event) {
  usernameInput.value.focus();
}
</script>

<style scoped lang="scss">
.loginContainer {
  .video {
    width: 100vw;
    height: 100vh;
    min-width: 100vw;
    object-fit: fill;
    position: absolute;
    top: 0;
    left: 0;
  }
  width: 100vw;
  height: 100vh;
  // background-color: rgb(0, 0, 0);
  background-image: url("../../assets/images/background.jpg");
  background-position: center center;
  background-repeat: no-repeat;
  background-attachment: fixed;
  background-size: cover;
  display: flex;
  justify-content: center;
  align-items: center;
  // .video-background {
  //   position: absolute;
  //   top: 0;
  //   left: 0;
  //   width: 100%;
  //   height: 100%;
  //   object-fit: cover;
  //   z-index: -1;
  // }
  .login {
    -webkit-user-select: none;
    /*谷歌 /Chrome*/
    -moz-user-select: none;
    /*火狐/Firefox*/
    -ms-user-select: none;
    /*IE 10+*/
    user-select: none;
    width: 20rem;
    border: 1px solid rgb(230, 230, 230);
    border-radius: 10px;
    position: absolute;
    padding: 50px;
    animation: mytran 0.7s linear;

    .header {
      text-align: center;
      font-weight: 800;
      font-size: 1.5rem;
      margin: -20px auto 15px;
      color: rgb(255, 255, 255);
    }
    .btn {
      width: 70%;
      display: block;
      margin: 30px auto -15px;
    }
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
}
</style>
