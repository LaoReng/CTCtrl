<template>
  <div class="homeContainer">
    <!-- 遮罩层 -->
    <div
      class="mask"
      v-show="
        positionList.is_show_mask && positionList.transition_delay == true
      "
      :style="
        'width:' +
        mask_width +
        'left:' +
        mask_left +
        'height:' +
        mask_height +
        'top:' +
        mask_top
      "
    ></div>
    <div class="box-img" @mousedown="handleMouseDown">
      <!-- 根据数据提示是否显示加载中 -->
      <Loading v-if="homestore.isLoading" />
      <!-- 控制列表显示 -->
      <div v-else class="ListShow">
        <div v-if="homestore.equipInfo.equipments != null">
          <el-checkbox-group v-model="selectedState" class="checkboxGroup">
            <el-checkbox
              v-for="(item, index) in homestore.equipInfo.equipments"
              :key="index"
              :value="item.id"
              :class="
                selectedState.includes(item.id as never)
                  ? 'checkbox aa-active'
                  : 'checkbox'
              "
              @click="checkItem($event, item)"
              @dblclick="doubleClick(item, index)"
            >
              <el-card
                @contextmenu="rightClick($event, item, index)"
                :disabled="item.eStatus == 2"
                :class="{
                  'box-card-mang': item.eStatus == 2,
                  'box-card-you': item.eStatus == 1,
                }"
              >
                <template v-slot:header>
                  <div class="clearfix">
                    <span style="font-size: 1.2rem">{{ item.name }}</span>
                    <!-- 判断状态 -->
                    <div class="left" v-if="item.eStatus == 1">
                      <Circle
                        :color="'rgb(4, 102, 4)'"
                        :borderColor="'rgb(119, 204, 119)'"
                      />
                      <span class="left_Text">在线</span>
                    </div>
                    <div class="right" v-else>
                      <Circle
                        :color="'rgb(255, 60, 92)'"
                        :borderColor="'rgb(255, 113, 113)'"
                      />
                      <span class="right_Text">繁忙</span>
                    </div>
                  </div>
                </template>

                <div class="text">
                  <div>IP地址：{{ item.ip }}</div>
                  <div>用户名：test用户</div>
                  <div>
                    系&nbsp;&nbsp;&nbsp;统：{{
                      item.type == 1 ? "windows" : "嵌入式"
                    }}
                  </div>
                </div>
              </el-card>
              <span v-show="selectedState.includes(item.id as never)"></span>
            </el-checkbox>
          </el-checkbox-group>
        </div>
        <div v-else style="width: 100%">
          <el-empty style="width: 100%; height: 80vh" description="暂无设备">
            <el-link type="info">
              <RouterLink target="_blank" to="/desc">查看使用说明</RouterLink>
            </el-link>
          </el-empty>
        </div>
      </div>
    </div>
    <!-- 居中显示秘钥 -->
    <!-- 显示弹窗情况是根据正式环境还是测试环境来进行判断 -->
    <el-dialog
      v-if="isShowDialog"
      v-model="centerDialogVisible"
      :title="`请输入连接${currentEquipmentName}设备秘钥`"
      width="300"
      draggable
      align-center
      @closed="ruleForm.secretInput = ''"
    >
      <el-form ref="ruleFormRef" :rules="rules" :model="ruleForm">
        <el-form-item prop="secretInput">
          <el-input
            v-model="ruleForm.secretInput"
            type="password"
            show-password
          ></el-input>
        </el-form-item>
      </el-form>
      <template #footer>
        <div class="dialog-footer">
          <el-button @click="centerDialogVisible = false">取消</el-button>
          <el-button type="primary" @click="confirmSecretCallBack">
            确定
          </el-button>
        </div>
      </template>
    </el-dialog>
    <el-dialog
      draggable
      width="450"
      align-center
      v-model="centerDialogVisible"
      v-else
    >
      <template #header>
        <div style="text-align: center; font-size: 20px">受限说明</div>
        <div style="margin-top: 20px; line-height: 25px">
          由于域名端口请求访问次数限制，当前访问只允许部分功能使用完整功能请访问
          <div style="display: flex; margin-left: 20px">
            <p style="">web前台地址：</p>
            <el-link :href="qianAddr" target="_blank">
              {{ qianAddr }}
            </el-link>
          </div>
          <div style="display: flex; margin-left: 20px">
            <p>后台管理地址：</p>
            <el-link :href="backAddr" target="_blank">
              {{ backAddr }}
            </el-link>
          </div>
        </div>
      </template>
    </el-dialog>
  </div>
</template>

<script lang="ts" setup>
import Circle from "@/components/common/Circle.vue";
import HomeStore from "@/store/modules/home/home";
import { ref, reactive, computed, onMounted, watch } from "vue";
//引入公共样式
import { detailStyleMenu } from "@/styles/home";
//引入Loading组件
import Loading from "@/components/common/Loading.vue";
import { ElMessage } from "element-plus";
import { useRouter } from "vue-router";
import { GET_TOKEN } from "@/utils/token";
import { menusEvent } from "vue3-menus";
import SettingStore from "@/store/modules/setting";
import { nextTick } from "vue";
const homestore = HomeStore();
homestore.init();
const settingStore = SettingStore();
//这里是部署的链接地址
let backAddr = ref("http://118.190.96.96:8090");
let qianAddr = ref(" http://118.190.96.96:9090");
let $router = useRouter();
//存储当前连接设备的名字
let currentEquipmentName = ref("");
//获取表单校验对象，这里用于秘钥校验
let ruleFormRef = ref<any>();
//表单密钥输入
let ruleForm = reactive({
  secretInput: "",
});
//表单校验
const rules = {
  secretInput: [{ required: true, message: "请输入密钥", trigger: "blur" }],
};
//是否显示弹出框
const centerDialogVisible = ref(false);
//如果是开发环境或者演示环境，那就就显示秘钥(true)的情况，如果是最终的山东比赛环境，就去ip访问
let currentEnvir = import.meta.env.VITE_ENV;
let isShowDialog = ref(
  currentEnvir == "dev" || currentEnvir == "test" ? true : false,
);
// 上方div图片是否被多选中，如果多选了就把id放到这个列表里面
const selectedState = ref([]);
//是否鼠标悬浮至详细信息
let isClickDetail = ref(false);
//存储当前的设备名字和索引
let saveCurrentEquipObj = ref({
  name: "",
  index: "",
});
//这里是计算框选的x和y
let dx = ref();
let dy = ref();
//计算box-img盒子的距离顶部的距离
let marginTopEle = ref<number>(0);
// 这个是上边小div的列表数据，后台数据请求回来

//双击回调
const doubleClick = async (name: any, index: any) => {
  console.log(name, index);
  //再次判断当前卡片的状态
  await homestore.init();
  //如果是繁忙状态，不让双击
  if (name.eStatus == 2) {
    saveCurrentEquipObj.value.name = name.name;
    saveCurrentEquipObj.value.index = index;
    ElMessage({
      // 可关闭
      showClose: true,
      message: "当前设备正在被控制，请稍后再试~",
      type: "error",
      duration: 1000,
    });
    return;
  }
  //如果当前获取不到token显示请登录
  if (GET_TOKEN() == null) {
    ElMessage({
      // 可关闭
      showClose: true,
      message: "请登录",
      type: "error",
    });
    return;
  }
  saveCurrentEquipObj.value.name = name.name;
  saveCurrentEquipObj.value.index = index;
  centerDialogVisible.value = true;
};
//点击密钥确定的回调和双击的回调
const confirmSecretCallBack = async () => {
  await ruleFormRef.value.validate(async (res: any) => {
    //如果输入秘钥。接下来就判断秘钥是否正确
    if (res) {
      console.log("输入了秘钥");
      //关闭对话框，但是在此之前要判断输入的秘钥是否正确，才能进行控制的情况
      centerDialogVisible.value = false;
      //输入秘钥正确，发起控制
      // console.log(saveCurrentEquipObj.value);
      let { name, index } = saveCurrentEquipObj.value;
      // console.log(name, index);
      console.log(`%c 密钥输入为：${ruleForm.secretInput}`, "color:red");
      // console.log(saveCurrentEquipObj.value.name, index);
      clickVnc(name, index);
    } else {
      return;
    }
  });
};

let wsData = import.meta.env.VITE_APP_WS_URL;
// 假设item结构包含name属性
const rightClick = (event: MouseEvent, item: any, index: any) => {
  // 修改菜单结构以支持动态数据传递
  const dynamicMenus = [
    {
      label: "连接",
      tip: "连接被控端",
      click: () => {
        //如果当前用户token（手机号）消失
        if (GET_TOKEN() == null) {
          ElMessage({
            // 可关闭
            showClose: true,
            message: "请登录",
            type: "error",
          });
          return;
        }
        //让弹出框显示
        // console.log("弹出框显示");
        // console.log(item, index);
        currentEquipmentName.value = item.name;
        centerDialogVisible.value = true;

        // return;
        //把点击连接的卡片名字和索引保存
        // Object.assign(saveCurrentEquipObj.value, item);
        // saveCurrentEquipObj.value.name = item.name;
        // saveCurrentEquipObj.value.index = index;
        saveCurrentEquipObj.value.name = item.name;
        saveCurrentEquipObj.value.index = index;
        // console.log(saveCurrentEquipObj.value);
        //第一个参数为校验的表单，第二个参数为点击卡片的名字，第三个参数为点击卡片的索引
        // confirmSecretCallBack();
      },
    },
    {
      label: "文件浏览",
      tip: "预览被控端文件",
      click: () => {
        if (GET_TOKEN() == null) {
          ElMessage({
            // 可关闭
            showClose: true,
            message: "请登录",
            type: "error",
          });
          return;
        }
        homestore.rightClickPreviewDeviceName = item.name;
        //嗅探包
        const ws = new WebSocket(wsData);

        // //对方开启websocket
        // ws.addEventListener("open", () => {
        //   homestore.isShowFileSystemDialog = true;
        // });
        // ws.addEventListener("error", () => {
        //   console.log("连接失败");
        // });
        // ws.close();
        // console.log(ws.readyState);
        // homestore.isShowFileSystemDialog = true;

        //TODO
        // const ws = new WebSocket("ws://43.139.168.168:9002");

        ws.addEventListener("open", () => {
          // console.log("客户端连接上服务端了");
          homestore.isShowFileSystemDialog = true;
        });
        // 如果需要处理连接失败或关闭的情况，也可以添加相应的事件监听器
        ws.addEventListener("error", (event) => {
          console.error("连接发生错误:", event);
          // homestore.isShowFileSystemDialog = false;
          homestore.isShowFileSystemDialog = true;
        });

        ws.addEventListener("close", () => {
          console.log("连接已关闭");
          homestore.isShowFileSystemDialog = false;
          ElMessage({
            message: "当前连接发生错误，请稍后再试",
            type: "error",
          });
        });
      },
    },
    {
      label: "详细信息",
      enter: () => {
        isClickDetail.value = true;
        console.log("详细信息卡片名称:", item.name, index); // 这里可以访问到卡片名称
      },
      children: [
        {
          label: `当前设备：${item.name}`,
          disabled: true,
          style: detailStyleMenu,
        },
        {
          label: `当前ip：${item.ip}`,
          disabled: true,
          style: detailStyleMenu,
        },
        {
          label: `设备名：${item.name}`,
          disabled: true,
          style: detailStyleMenu,
        },
        {
          label: `设备状况：${item.eStatus == 1 ? "在线" : "繁忙"}`,
          disabled: true,
          style: detailStyleMenu,
        },
        {
          label: `设备所属地区：${"黑龙江"}`,
          disabled: true,
          style: detailStyleMenu,
        },
      ],
    },
  ];

  menusEvent(event, dynamicMenus, 0); // 使用修改后的菜单结构
  event.preventDefault();
};

//点击卡片的回调
// eslint-disable-next-line @typescript-eslint/no-unused-vars
async function clickVnc(name: any, index: any) {
  try {
    //再次判断当前卡片的状态
    await homestore.init();
    //如果当前的设备状态是2，就表明已经被占用
    if (homestore.equipInfo.equipments?.[index].eStatus == 2) {
      return;
    }
    //如果当前用户token（手机号）消失
    if (GET_TOKEN() == null) {
      ElMessage({
        // 可关闭
        showClose: true,
        message: "请登录",
        type: "error",
      });
      return;
    } else {
      // return;
      //否则仓库发起控制设备的参数
      homestore.requestParam.ctrlerPhone = GET_TOKEN() as string;
      homestore.requestParam.bectrlEquipName = name;
      homestore.requestParam.startCtrlTime = new Date()
        .toLocaleString()
        .replaceAll("/", "-");
      homestore.requestParam.key = ruleForm.secretInput;
      console.log("发起控制设备的请求参数", homestore.requestParam);
      //发起设备控制
      let res = await homestore.startControl();
      console.log(res);
      //密码输入正确
      if (res.eStatus != 2) {
        console.log("%c输入密钥正确", "color:green");
        //如果是嵌入式设备
        if (homestore.equipInfo.equipments[index].type == 2) {
          let routeData = $router.resolve({
            path: "/embed",
          });
          // 监听浏览器的变化
          let page = window.open(routeData.href, "openhsWindows");
          const loop = setInterval(async () => {
            if (page?.closed) {
              // let res = await get("http://localhost:8888/test");
              await homestore.endTime({ bectrlEquipName: name });
              clearInterval(loop);
              console.log("%c嵌入式标签页关闭", "color:pink");
            }
          }, 1000);
          homestore.init();
        }
        //windows
        else {
          let routeData = $router.resolve({
            name: "vnc",
            query: {
              name: name,
            },
          });
          // 监听浏览器的变化
          let page = window.open(routeData.href, "openhsWindows");
          //标签页关闭
          const loop = setInterval(async () => {
            if (page?.closed) {
              // let res = await get("http://localhost:8888/test");
              await homestore.endTime({ bectrlEquipName: name });
              clearInterval(loop);
              console.log("%cwindows标签页关闭", "color:blue");
            }
          }, 1000);
          homestore.init();
        }
      } else {
        ElMessage({
          type: "error",
          message: "输入密钥不正确",
          showClose: true,
          duration: 1600,
        });
      }
    }
  } catch (error) {
    console.log(error);
    ElMessage({
      type: "error",
      message: "输入密钥不正确",
      showClose: true,
      duration: 1600,
    });
  }
}

//计算遮罩层的初始数据
const positionList = reactive({
  is_show_mask: false,
  box_screen_left: 0,
  box_screen_top: 0,
  start_x: 0,
  start_y: 0,
  end_x: 0,
  end_y: 0,
  scroll: 0,
  transition_delay: true, //是否延迟显示
});
//分别计算遮罩层的位置，大小
let mask_width = computed(() => {
  return `${Math.abs(positionList.end_x - positionList.start_x)}px;`;
});
let mask_height = computed(() => {
  return `${Math.abs(positionList.end_y - positionList.start_y)}px;`;
});
let mask_left = computed(() => {
  return `${Math.min(positionList.start_x, positionList.end_x) - positionList.box_screen_left}px;`;
});
let mask_top = computed(() => {
  return `${Math.min(positionList.start_y, positionList.end_y) - positionList.box_screen_top}px;`;
});
//单击多选框选中
let checkItem = (_val: any, item: any) => {
  let str: any = item.id;
  let i = selectedState.value.indexOf(str as never); // 判断选中列表中是否包含这个点击的div
  if (i < 0) {
    selectedState.value.push(str as never); // 如果不包含就加进去
  } else {
    selectedState.value.splice(i, 1); // 如果包含就删
  }
};

//当点击了缩小栏，我们给其重新获取DOM大小
watch(
  () => settingStore.fold,
  () => {
    //在此过渡阶段，不让其滑动框选
    positionList.transition_delay = false;
    nextTick(() => {
      setTimeout(() => {
        let contentEle: any = document.querySelector(".content");
        dx.value = contentEle?.offsetLeft;
        dy.value = contentEle?.offsetTop;
        console.log(dx.value, dy.value);
        positionList.transition_delay = true;
      }, 1000);
    });
  },
);

//鼠标按下事件
const handleMouseDown = (event: any) => {
  //拦截鼠标的中键和右键，只有在方块内才能右键
  if (event.which != 1) {
    return;
  }
  event.preventDefault();

  positionList.is_show_mask = true;
  positionList.start_x = event.pageX - dx.value;
  positionList.start_y = event.pageY - dy.value;
  positionList.end_x = event.pageX - dx.value;
  positionList.end_y = event.pageY - dy.value;
  document
    .querySelector(".content")
    ?.addEventListener("mousemove", handleMouseMove); //监听鼠标移动事件
  document
    .querySelector(".content")
    ?.addEventListener("mouseup", handleMouseUp); //监听鼠标抬起事件
  // document
  //   .querySelector(".content")
  //   ?.addEventListener("mousewheel", handleScroll); //监听鼠标抬起事件
};

//处理鼠标移动
function handleMouseMove(event: any) {
  // console.log(event.offsetX, event.offsetY);
  positionList.end_x = event.pageX - dx.value;
  positionList.end_y = event.pageY - dy.value;
  // console.log(positionList.end_x, positionList.end_y);
}

//处理鼠标抬起
function handleMouseUp() {
  document
    .querySelector(".content")
    ?.removeEventListener("mousemove", handleMouseMove);
  document
    .querySelector(".content")
    ?.removeEventListener("mouseup", handleMouseUp);
  positionList.is_show_mask = false;

  handleDomSelect();
  resSetXY();
  // console.log(selectedState.value);
  //将当前框选的卡片信息传给仓库homestore
  homestore.selectedCard = [...selectedState.value];
}

//初始化
onMounted(() => {
  let contentEle: any = document.querySelector(".content");
  dx.value = contentEle?.offsetLeft;
  dy.value = contentEle?.offsetTop;
  console.log(dx.value, dy.value);
  let ele: any = document.querySelector(".box-img");
  // console.log(ele.offsetTop);
  //这里是计算box-img距离顶部的距离
  marginTopEle.value = ele.offsetTop;
});

//鼠标选择
function handleDomSelect() {
  const dom_mask = window.document.querySelector(".mask");
  //getClientRects()每一个盒子的边界矩形的矩形集合
  const rect_select = dom_mask?.getClientRects()[0];
  //   console.log(rect_select);
  const add_list: any[] = [];
  const del_list: any[] = [];
  document
    .querySelectorAll(".el-checkbox-group .el-checkbox")
    .forEach((node, index) => {
      const rects = node.getClientRects()[0];
      // console.log(rects);
      if (collide(rects, rect_select) === true) {
        if (
          selectedState.value.includes(
            homestore.equipInfo.equipments[index].id as never,
          )
        ) {
          del_list.push(homestore.equipInfo.equipments[index].id);
        } else {
          add_list.push(homestore.equipInfo.equipments[index].id);
        }
      }
    });
  selectedState.value = selectedState.value
    .concat(add_list as any)
    .filter((item) => !del_list.includes(item));
}

//! 鼠标滚动
// eslint-disable-next-line @typescript-eslint/no-unused-vars
function handleScroll() {
  let ele: any = document.querySelector(".content");
  console.log(ele.scrollTop); //获取滚动的高度
  //把这个高度和框选结合
  // return ele.scrollTop;
  positionList.scroll = ele.scrollTop;
}
//比较checkbox盒子边界和遮罩层边界最大最小值
function collide(rect1: any, rect2: any) {
  const maxX = Math.max(rect1.x + rect1.width, rect2.x + rect2.width);
  const maxY = Math.max(rect1.y + rect1.height, rect2.y + rect2.height);
  const minX = Math.min(rect1.x, rect2.x);
  const minY = Math.min(rect1.y, rect2.y);
  return (
    maxX - minX <= rect1.width + rect2.width &&
    maxY - minY <= rect1.height + rect2.height
  );
}

//清除
function resSetXY() {
  positionList.start_x = 0;
  positionList.start_y = 0;
  positionList.end_x = 0;
  positionList.end_y = 0;
}
</script>
<style lang="scss" scoped>
.homeContainer {
  --margintop: v-bind(marginTopEle);
  height: calc(100vh - $header - 18px - var(--margintop));
  width: 100%;
  // background-color: blue;
  .box-img {
    position: relative;
    height: calc(100vh - $header - 18px - var(--margintop));
    width: 100%;
    .ListShow {
      // 测试
      height: calc(100vh - $header - 2.3rem);
      // width: 100vw;
    }
  }
  .mask {
    position: absolute;
    background: #409eff;
    opacity: 0.4;
    z-index: 100;
  }
}

.checkboxGroup {
  display: flex;
  flex-wrap: wrap;
  .el-checkbox {
    margin: 10px;
    width: 17rem;
    border: none;
    .text {
      div {
        margin-bottom: 3px;
      }
    }
    .clearfix {
      display: flex;
      justify-content: space-between;
      align-items: center;
      height: 2rem;

      .left {
        display: flex;
        align-items: center;

        span {
          font-size: 1rem;
        }

        /* height: 90px; */
        .left_Text {
          margin-left: 1rem;
        }
      }

      .right {
        display: flex;
        align-items: center;
        height: 90px;

        p {
          font-size: 30px;
          margin-right: 5px;
          margin-top: 32px;
        }

        span {
          font-size: 1rem;
        }

        .right_Text {
          margin-left: 1rem;
        }
      }
    }
  }
  ::v-deep .el-checkbox__inner {
    border-radius: 100%;
  }
  ::v-deep .el-checkbox__input {
    white-space: nowrap;
    cursor: pointer;
    outline: 0;
    display: inline-flex;
    position: absolute;
    right: 5px;
    bottom: 5px;
  }

  .el-checkbox__label {
    padding: 0;
    width: 100%;
    height: 100%;
  }
  .el-checkbox__input .el-checkbox__inner:after {
    border-color: #00ffff;
  }
  .el-checkbox__input.is-checked .el-checkbox__inner {
    border: 1px solid #00ffff;
  }
  .el-checkbox__inner {
    border: 1px solid #fff;
  }
}
.checkbox {
  border: 1px solid #999;
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: space-around;
  position: relative;
}
.checkbox-active {
  border: 1px solid #00ffff;
}
/* 在线样式 */
.box-card-you {
  width: 17rem;
  cursor: pointer;
  background-image: linear-gradient(to bottom, #a1c4fd 0%, #c2e9fb 100%);
  /* background-color: rgb(220, 220, 220); */
}
/* 繁忙样式 */
.box-card-mang {
  cursor: default;
  pointer-events: none; // 禁止点击事件

  width: 17rem;
  //   cursor: pointer;
  //   pointer-events: none;
  /* background-image: linear-gradient(to bottom, #a1c4fd 0%, #c2e9fb 100%); */
  background-color: rgb(220, 220, 220);
}
// 繁忙状况下不让其点击
.box-card-mang:hover {
  cursor: not-allowed;
  //   pointer-events: none;
}
</style>
