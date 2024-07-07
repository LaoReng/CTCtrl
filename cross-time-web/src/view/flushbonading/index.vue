<template>
  <div class="container">
    <vue-particles
      id="tsparticles"
      :particlesInit="particlesInit"
      :particlesLoaded="particlesLoaded"
      :options="data.options"
    />
    <div class="door">
      <!-- 门← -->
      <div class="left_door">
        <div class="handle">
          <div class="left"></div>
          <div class="right"></div>
        </div>
      </div>
      <!-- 门→ -->
      <div class="right_door">
        <!-- <div class="box">123</div> -->
        <div class="handle">
          <div class="left"></div>
          <div class="right"></div>
        </div>
        <div class="screen" ref="screen">
          <!-- 显示内容 -->
          <div class="view">
            <div style="margin-top: 5px"></div>
            <ViewMain />
          </div>
          <!-- 导航栏 -->
          <div class="tabbar">
            <button class="home" @click="$router.push('/embed/home')">
              主页面
            </button>
            <button class="left" @click="$router.push('/embed/about')">
              返回
            </button>
          </div>
        </div>
      </div>

      <!-- 内容 -->
      <div class="content">
        <div class="leftDoorContent">
          <LeftContent />
        </div>
        <div class="rightDoorContent">
          <RightContent />
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { loadFull } from "tsparticles";
import LeftContent from "./view/refrigeration/LeftContent.vue";
import RightContent from "./view/freeze/RightContent.vue";
import { onMounted } from "vue";
import { useRouter } from "vue-router";
import { ref, reactive } from "vue";
import ViewMain from "./main/ViewMain.vue";
let $router = useRouter();
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
const particlesInit = async (engine: any) => {
  // console.log(engine);
  await loadFull(engine);
};
const particlesLoaded = async (container: any) => {
  console.log("Particles container loaded", container);
};
let screen = ref();
let LFlag = ref(false); //左把手的锁
let RFlag = ref(false); //右把手的锁
//打开左门
const LeftOpenDoor = () => {
  let left_door = document
    .querySelector(".left_door")
    ?.querySelector(".handle");
  let doorL: any = document.querySelector(".door")?.querySelector(".left_door");
  left_door?.addEventListener("click", () => {
    LFlag.value = !LFlag.value;
    LFlag.value
      ? (doorL.style.transform = "translateX(-100%)")
      : (doorL.style.transform = "translateX(0%)");
    // console.log(LFlag.value);
  });
};
//打开右门
const RightOpenDoor = () => {
  let right_door = document
    .querySelector(".right_door")
    ?.querySelector(".handle");
  let doorL: any = document
    .querySelector(".door")
    ?.querySelector(".right_door");
  right_door?.addEventListener("click", () => {
    RFlag.value = !RFlag.value;
    RFlag.value
      ? (doorL.style.transform = "translateX(100%)")
      : (doorL.style.transform = "translateX(0%)");
    // console.log(RFlag.value);
  });
};
onMounted(() => {
  // console.log(screen.value);
  LeftOpenDoor();
  RightOpenDoor();
});
</script>

<style scoped lang="scss">
.container {
  outline: none;
  // background-color: rgb(255, 233, 233);
  background-image: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
  color: white;
  width: 100%;
  height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;

  /*门里面*/
  .door {
    width: 500px;
    height: 700px;
    background-color: rgb(255, 255, 255);
    background-size: 100% 100%;
    perspective: 1500px;
    position: relative;
    display: flex;
    .left_door {
      box-shadow: 0 15px 35px rgb(108, 108, 108);
      transform-origin: left center;
      border-right: 1px solid rgb(8, 8, 8);
      position: relative;
      opacity: 1;
      z-index: 10;
      .handle {
        position: absolute;
        top: 50%;
        right: 0%;
        transform: translate(0, -50%);
        width: 20px;
        height: 90px;
        border: 2px solid black;
        display: flex;
        .left {
          flex: 1;
          background-color: black;
          border-left: 1px solid #000;
          border-top: 1px solid #000;
          border-right: 3px solid #000;
          border-bottom: 3px solid #000;
        }
        .right {
          background-color: rgb(192, 192, 192);
          border-right: 1px solid rgb(192, 192, 192);
          border-bottom: 1px solid rgb(192, 192, 192);
          border-left: 3px solid rgb(192, 192, 192);
          border-top: 3px solid rgb(192, 192, 192);
          flex: 1;
        }
      }
    }

    .right_door {
      box-shadow: 0 15px 35px rgb(108, 108, 108);
      z-index: 10;
      position: relative;
      transform-origin: right center;
      border-left: 1px solid rgb(8, 8, 8);
      .handle {
        position: absolute;
        top: 50%;
        left: 0%;
        transform: translate(0, -50%);
        width: 20px;
        height: 90px;
        border: 2px solid black;
        display: flex;
        .left {
          background-color: rgb(192, 192, 192);
          border-right: 1px solid rgb(192, 192, 192);
          border-bottom: 1px solid rgb(192, 192, 192);
          border-left: 3px solid rgb(192, 192, 192);
          border-top: 3px solid rgb(192, 192, 192);
          flex: 1;
        }
        .right {
          background-color: black;
          border-left: 1px solid #000;
          border-top: 1px solid #000;
          border-right: 3px solid #000;
          border-bottom: 3px solid #000;
          flex: 1;
        }
      }
      .screen {
        position: absolute;
        left: 50%;
        top: 30%;
        transform: translate(-50%, -50%);
        background-color: #222222;
        border: 1px solid #ccc;
        width: 190px;
        height: 300px;
        border-radius: 10px;
        .view {
          overflow-y: scroll;
          position: absolute;
          left: 50%;
          top: 20%;
          transform: translate(-50%, -20%);
          width: 90%;
          background-color: rgb(255, 255, 255);
          height: calc(300px - 10%);
          border-radius: 10px;
        }
        .view::-webkit-scrollbar {
          width: 0 !important;
        }
        .tabbar {
          height: calc(300px - 90%);
          width: 100%;
          display: flex;
          justify-content: space-around;
          align-items: center;
          position: absolute;
          bottom: 0;
          // background-color: red;
          font-size: 10px;
          color: rgb(0, 0, 0);
          button {
            background-color: white;
            font-family: "Lucida Sans", "Lucida Sans Regular", "Lucida Grande",
              Geneva;
            border-radius: 5px;
          }
          button:hover {
            background-color: #00b6ed;
            color: white;
            cursor: pointer;
          }
        }
      }
    }
    .content {
      position: absolute;
      width: 100%;
      display: flex;
      .leftDoorContent {
        flex: 1;
        // border-right: 1px solid red;
        // height: 100vh;
      }
      .rightDoorContent {
        flex: 1;
      }
    }
  }
  .left_door,
  .right_door {
    flex: 1;
    height: 100%;
    background: #757f9a; /* fallback for old browsers */
    background: -webkit-linear-gradient(
      to right,
      rgb(117, 127, 154),
      rgb(215, 221, 232)
    ); /* Chrome 10-25, Safari 5.1-6 */
    background: linear-gradient(
      to right,
      rgb(117, 127, 154),
      rgb(215, 221, 232)
    ); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */
    z-index: 1;
    transition: all 2s ease;
    transform-origin: left center;
  }

  // div.door div.left_door {
  //   transform: rotateY(-100deg);
  // }

  // div.door div.right_door {
  //   transform: rotateY(100deg);
  // }
}
</style>
