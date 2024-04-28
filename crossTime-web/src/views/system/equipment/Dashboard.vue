<template>
  <div style="min-width: 50rem;">
    <div v-if="isLoading == true">
      <div>
        <van-loading vertical>
          <template #icon>
            <van-icon name="star-o" size="30" />
          </template>
          加载中...
        </van-loading>
      </div>
    </div>
    <div v-else>
      <div v-if="equipInfo.equipments != null" style="display: flex;flex-wrap: wrap;">
        <el-card @click.native="clickVnc(item, index)" :disabled="item.eStatus == 2"
          :class="{ 'box-card-mang': item.eStatus == 2, 'box-card-you': item.eStatus == 1 }"
          v-for="(item, index) in equipInfo.equipments" :key="index" style="margin: 1vh">
          <div slot="header" class="clearfix">
            <span style="font-size: 18px"> {{ item.name }}</span>
            <!-- 判断状态 -->
            <div class="left" v-if="item.eStatus == 1" style="color: rgb(0, 176, 80); padding: 3px 0; display: flex"
              type="text">
              <p>•</p>
              <span style="color: black">在线</span>
            </div>
            <div class="right" v-else style="
                color: rgb(255, 0, 0);
                padding: 3px 0;
                display: flex;
                color: red;
                padding: 3px 0;
              " type="text">
              <p>•</p>
              <span style="color: black">繁忙</span>
            </div>
          </div>

          <div class="text">
            <div>IP：{{ item.ip }}</div>
            <div>用户名：test用户</div>
          </div>
        </el-card>
      </div>
      <div v-else style="position: absolute;left: 55%;top: 50%;transform: translate(-50%,-50%);min-width: 50rem;">
        <el-empty description="暂无设备"></el-empty>
      </div>
    </div>
  </div>
</template>

<script>

import { get, post } from '@/plugins/request';
export default {
  data() {
    return {
      screenWidth: null,
      equipInfo: {},
      isLoading: true,
      requestParam: {
        ctrlerPhone: "",   // 控制者的手机号
        bectrlEquipName: "", // 被控设备名称
        startCtrlTime: ""    // 开始控制的时间xxxx-xx-xx xx:xx:xx
      }
    };
  },
  methods: {
    async init() {
      //初始化获取Vnc列表
      try {
        let res = await get("/equipmentlist");
        console.log("当前Vnc列表状态", res);
        // this.equipInfo
        this.isLoading = false;
        this.equipInfo = { ...this.equipInfo, ...res };
      } catch (error) {
        this.$message({
          // 可关闭
          showClose: true,
          message: "后台服务器繁忙",
          type: "error",
        });
      }
    },

    //点击卡片，跳转到Vnc页面
    async clickVnc(e, index) {

      //再次判断当前状态
      let result = await get("/equipmentlist");
      this.equipInfo = { ...result };
      if (result.equipments[index].eStatus == 2) {
        return;
      }

      //这个e是获取点击的信息
      console.log("点击的信息状况：", e);
      //如果是繁忙状态，返回不让点击
      // if (e.eStatus == 2) return;
      // console.log(this.equipInfo);

      this.requestParam.ctrlerPhone = localStorage.getItem("token");
      this.requestParam.bectrlEquipName = e.name;
      this.requestParam.startCtrlTime = new Date().toLocaleString().replaceAll("/", "-");

      //如果当前用户token（手机号）消失
      if (this.requestParam.ctrlerPhone == null) {
        this.$message({
          // 可关闭
          showClose: true,
          message: "当前登录失效，请重新登录",
          type: "error",
        });

        setTimeout(() => {
          this.$router.push("/login");
        }, 2000)
      }
      //发送post请求跳转vnc页面
      else {

        let res = await post("startEquipmentCtrl", this.requestParam);
        localStorage.setItem("startEquipment", JSON.stringify(res));//存储下来，之后可以通过本地存储获取下来
        let routeData = this.$router.resolve({
          name: 'vnc',
          query: {
            name: e.name
          }
        });
        //如果你确实希望模拟一个可点击的消息体并新开标签页： 
        this.$message({
          type: "info",
          title: '消息',
          message: ' 如遇不跳转情况，请手动点击标签页进行跳转...'
        });
        //监听浏览器的变化
        let page = window.open(routeData.href, 'openhsWindows');
        //标签页关闭
        var loop = setInterval(async () => {
          if (page.closed) {
            clearInterval(loop);
            console.log("标签页关闭");
            // let res = await get("http://localhost:8888/test");
            this.endTime({ bectrlEquipName: e.name })
          }
        }, 1000);

      }

      let res = await get("/equipmentlist");
      // console.log(res);
      // this.equipInfo
      this.isLoading = false;
      this.equipInfo = { ...this.equipInfo, ...res };
    },
    //结束连接
    async endTime(name) {
      await post("/endEquipmentCtrl", name);
      //重新获取数据
      this.init();
    },
  },
  mounted() {
    this.init();
  }
};
</script>

<style scoped>
/* 繁忙样式 */
.box-card-mang {
  width: 300px;
  cursor: pointer;
  /* background-image: linear-gradient(to bottom, #a1c4fd 0%, #c2e9fb 100%); */
  background-color: rgb(220, 220, 220);
}

.box-card-mang:hover {
  cursor: not-allowed;
  /* pointer-events: none; */
}

/* 在线样式 */
.box-card-you {
  width: 300px;
  cursor: pointer;
  background-image: linear-gradient(to bottom, #a1c4fd 0%, #c2e9fb 100%);
  /* background-color: rgb(220, 220, 220); */
}

.text {
  font-size: 20px;
  font-family: "微软雅黑";
}

.left {
  display: flex;
  align-items: center;
  height: 90px;
}

.right {
  display: flex;
  align-items: center;
  height: 90px;
}

.clearfix {
  display: flex;
  justify-content: space-between;
  align-items: center;
  height: 2rem;
}

.clearfix .left p {
  font-size: 30px;
  margin-right: 5px;
  margin-top: 32px;
}

.el-card__header {
  height: 8rem;
}

.clearfix .left span {
  font-size: 1rem;
}

.clearfix .right p {
  font-size: 30px;
  margin-right: 5px;
  margin-top: 32px;
}

.clearfix .right span {
  font-size: 1rem;
}

.clearfix:after {
  clear: both;
}

.el-card {
  border-radius: 2em;
}
</style>
