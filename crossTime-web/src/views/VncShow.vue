<template>
    <!-- 整体的页面容器 -->
    <div class="container" v-if="isShow">

        <div class="app-header">
            <div class="left">
                <img src="../assets/logo.jpg" style="width: 5rem" />
                <span style="font-size: 1.1rem;">跨时空控制台</span>
            </div>
            <div class="center">
                <el-statistic @change="currentDownTime"
                    :value-style="changeColor ? { color: 'red' } : { color: 'white' }" @finish="surplusTime"
                    :value="deadLine" format="HH:mm:ss" time-indices title="🚩剩余时间">
                </el-statistic>
            </div>
            <div class="right">
                <div class="btn">
                    <i @click="finishControl" class="el-icon-switch-button"></i>
                </div>
            </div>
        </div>

        <!-- 显示区域 -->
        <!-- <div class="ShowArea"
            :style="{ width: currentWindowSize.width + 'px', height: currentWindowSize.height + 'px', padding: 0, minWidth: '72rem', minHeight: '42rem' }">
            <div>
                <img class="screen" style="width: 100%;height: 100%;" :src="imgUrl" alt="">
            </div>
           
            <div class="right" style="width: 4.6rem">
                <div>
                    <el-checkbox-group fill="#585eaa" v-model="checkboxGroup">
                        <el-checkbox-button @change="changeCheckbox" v-for="(item, index) in event" :label="item.name"
                            :key="index">
                            <img :src="item.icon">
                        </el-checkbox-button>
                    </el-checkbox-group>
                </div>
                <div>
                    <el-button>
                        <img :src="require('../assets/文件夹.png')" alt="">
                    </el-button>
                </div>
            </div>
        </div> -->

        <div class="ShowArea">
            <div class="imgStyle">
                <img class="screen" style="width: 100%;height: 100%;" :src="imgUrl" alt="">
            </div>
            <!-- 侧边栏 -->
            <div class="right" style="width: 4vw;min-width: 4rem;">
                <div>
                    <el-checkbox-group fill="#585eaa" v-model="checkboxGroup">
                        <el-checkbox-button @change="changeCheckbox" v-for="(item, index) in event" :label="item.name"
                            :key="index">
                            <img :src="item.icon">
                        </el-checkbox-button>
                    </el-checkbox-group>
                </div>
                <div>
                    <el-button>
                        <img :src="require('../assets/文件夹.png')" alt="">
                    </el-button>
                </div>
            </div>
        </div>
    </div>
    <div v-else>加载中...</div>

</template>
<script>

import { get, post } from '@/plugins/request';
export default {
    name: "VncShow",
    data() {
        return {
            keepConstant: null,
            //让倒计时变色
            changeColor: false,
            //是否初始化显示页面数据
            isShow: false,
            //存储基础属性
            baseInfo: {},
            imgUrl: "",
            miceFlag: false,//鼠标锁
            keyFlag: false,//键盘锁
            event: [
                {
                    name: "鼠标",
                    icon: require("../assets/鼠标.png"),
                    isChecked: false,

                },
                {
                    name: "键盘",
                    icon: require("../assets/键盘.png"),
                    isChecked: false,

                }
            ],
            //截止时间
            deadLine: Date.now(),
            checkboxGroup: [],
            //初始化将图片根据窗口进行比例转换
            //我们的图片是通过localStorage进行存储的。
            imgParam: {},
            //当前鼠标的坐标位置
            miceLocation: {
                locationX: 0,
                locationY: 0,
            },
            //当前窗口的大小
            currentWindowSize: { width: 0, height: 0 },
            //鼠标按下事件保存
            clickDownEventListener: null,
            //鼠标抬起事件保存
            clickUpEventListener: null,
            //键盘按下事件保存
            keywordDownEventListener: null,
            //键盘抬起事件保存
            keywordUpEventListener: null,
            //鼠标滑动事件保存
            mouseMoveEventListener: null,
            //鼠标滚动事件保存
            mouseScrollEventListener: null,
            //离开时间保存
            beforeUnloadEventListener: null,
        }
    },
    //方法区
    methods: {

        //倒计时变色
        currentDownTime(val) {
            //小于59s
            if (Math.floor((val - (1000 * 60 * 1)) / 1000) < 0) {
                this.changeColor = true;//
            }
            this.keepConstant = val;

        },
        //结束时间事件触发
        surplusTime(val) {
            if (val) {
                this.commonHandleFinish();
            }
        },
        //比特位操作，方便键盘鼠标的一些操作传递信息
        setBitInByte(byte, n) {
            return byte | (1 << n);
        },
        getBitFromByte(byte, n) {
            return (byte >> n) & 1;
        },
        //鼠标按下事件
        addClickDownHandler() {
            const screenElement = document.querySelector('.screen');
            const clickEventHandler = async (ev) => {
                // console.log("您点击了", ev);
                //点击了左键
                if (ev.button == 0) {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 4);//按下
                    // console.log(byte);//16
                    byte = this.setBitInByte(byte, 2);//左键
                    console.log(byte);
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })

                }
                //点击了中键
                else if (ev.button == 1) {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 4);//按下
                    byte = this.setBitInByte(byte, 1);//中键
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })
                    // console.log("鼠标中键的请求结果", res);
                }
                //点击了右键
                else {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 4);//按下
                    byte = this.setBitInByte(byte, 0);//右键
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })
                    // console.log("鼠标右键的请求结果", res);
                }
            }
            screenElement.addEventListener("mousedown", clickEventHandler)
            this.clickDownEventListener = clickEventHandler;
        },
        //鼠标按下清除事件
        removeClickDownHandler() {
            // 移除之前添加的点击事件监听器
            const screenElement = document.querySelector('.screen');
            if (this.clickDownEventListener) {
                screenElement.removeEventListener('mousedown', this.clickDownEventListener);
                // 清理引用，防止内存泄漏
                this.clickDownEventListener = null;
            }
        },
        //鼠标按下抬起事件
        addClickUpHandler() {
            const screenElement = document.querySelector('.screen');
            const clickEventHandler = async (ev) => {
                // console.log("您抬起了鼠标", ev);
                //点击了左键
                if (ev.button == 0) {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 5);//抬起
                    // console.log(byte);//16
                    byte = this.setBitInByte(byte, 2);//左键
                    console.log(byte);
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })

                }
                //点击了中键
                else if (ev.button == 1) {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 5);//抬起
                    byte = this.setBitInByte(byte, 1);//中键
                    let res = await post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })
                    console.log("鼠标中键的请求结果", res);
                }
                //点击了右键
                else {
                    let byte = 0b00000000;
                    byte = this.setBitInByte(byte, 5);//抬起
                    byte = this.setBitInByte(byte, 0);//右键
                    let res = await post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: 0
                    })
                    console.log("鼠标右键的请求结果", res);
                }
            }
            screenElement.addEventListener("mouseup", clickEventHandler)
            this.clickUpEventListener = clickEventHandler;
        },
        //鼠标按下抬起清除事件
        removeClickUpHandler() {
            // 移除之前添加的点击事件监听器
            const screenElement = document.querySelector('.screen');
            if (this.clickUpEventListener) {
                screenElement.removeEventListener('mouseup', this.clickUpEventListener);
                // 清理引用，防止内存泄漏
                this.clickUpEventListener = null;
            }
        },
        //键盘按下事件
        addKeyWordDownHandler() {
            const keywordElement = document;
            const keywordEventHandler = async (ev) => {
                // console.log("您按下了键盘", ev);
                let byte = 0b00000000;
                byte = this.setBitInByte(byte, 4);//按下
                // console.log(byte);
                await post("/ctrlEvent", {
                    bectrlEquipName: this.baseInfo.name,
                    EventType: 2,
                    MousePoint: { x: 0, y: 0 },
                    operation: byte,
                    Value: ev.keyCode
                })

            }
            keywordElement.addEventListener("keydown", keywordEventHandler);
            this.keywordDownEventListener = keywordEventHandler;
        },
        //键盘抬起事件
        addKeyWordUpHandler() {
            const keywordElement = document;
            const keywordEventHandler = async (ev) => {
                // console.log("您抬起了键盘", ev);
                let byte = 0b00000000;
                byte = this.setBitInByte(byte, 5);//抬起
                // console.log(byte);
                await post("/ctrlEvent", {
                    bectrlEquipName: this.baseInfo.name,
                    EventType: 2,
                    MousePoint: { x: 0, y: 0 },
                    operation: byte,
                    Value: ev.keyCode
                })
            }
            keywordElement.addEventListener("keyup", keywordEventHandler);
            this.keywordUpEventListener = keywordEventHandler;
        },
        //键盘按下清除事件
        removeKeyWordDownHandler() {
            // 移除之前添加的键盘事件监听器
            const keywordElement = document
            if (this.keywordDownEventListener) {
                keywordElement.removeEventListener('keydown', this.keywordDownEventListener);
                // 清理引用，防止内存泄漏
                this.keywordDownEventListener = null;
            }
        },
        //键盘抬起清除事件
        removeKeyWordUpHandler() {
            // 移除之前添加的键盘事件监听器
            const keywordElement = document
            if (this.keywordUpEventListener) {
                keywordElement.removeEventListener('keyup', this.keywordUpEventListener);
                // 清理引用，防止内存泄漏
                this.keywordUpEventListener = null;
            }
        },
        //鼠标滑动事件
        addMouseMoveHandler() {
            const mouseMoveElement = document.querySelector(".screen");

            const mouseMoveEventHandler = (ev) => {

                // console.log(this.imgParam.width);    
                // console.log("当前的鼠标坐标：", ev.offsetX, ev.offsetY);
                // console.log("图片大小", this.imgParam.width, this.imgParam.height);
                // console.log("当前桌面大小", this.currentWindowSize.width, this.currentWindowSize.height);
                this.miceLocation.locationX = Math.round((this.imgParam.width * ev.offsetX) / (document.querySelector('.screen').width));
                this.miceLocation.locationY = Math.round((this.imgParam.height * ev.offsetY) / (document.querySelector('.screen').height));
                // console.log("==================", this.imgParam.height, ev.offsetY, this.currentWindowSize.height);
                // console.log("您滑动了鼠标", Math.round(this.miceLocation.locationX), Math.round(this.miceLocation.locationY));
                post("/ctrlEvent", {
                    bectrlEquipName: this.baseInfo.name,
                    EventType: 1,
                    MousePoint: {
                        x: this.miceLocation.locationX, y: this.miceLocation.locationY
                    },
                    operation: 0,//!滑动
                    Value: 0//!滑动
                })
            }
            mouseMoveElement.addEventListener('mousemove', mouseMoveEventHandler);
            this.mouseMoveEventListener = mouseMoveEventHandler;



        },
        //鼠标滑动清除事件
        removeMouseMoveHandler() {
            const mouseMoveElement = document.querySelector('.screen');
            if (this.mouseMoveEventListener) {
                mouseMoveElement.removeEventListener("mousemove", this.mouseMoveEventListener);
                this.mouseMoveEventListener = null;
            }
        },
        //鼠标滚动事件
        addMouseScrollHander() {
            const mouseScrollElement = document.querySelector(".screen");
            // console.log("鼠标滚动");
            const mouseScrollEventHandler = async (ev) => {
                let byte = 0b00000000;
                byte = this.setBitInByte(byte, 6);//滚动
                if (ev.wheelDelta > 0) {
                    //下逻辑对应接口
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: ev.deltaY
                    }).then(r => {
                        console.log(r);
                    })

                }
                else {
                    //上逻辑对应接口
                    post("/ctrlEvent", {
                        bectrlEquipName: this.baseInfo.name,
                        EventType: 1,//1表示鼠标事件
                        MousePoint: { x: this.miceLocation.locationX, y: this.miceLocation.locationY },
                        operation: byte,//最终的bit数10进制
                        Value: ev.deltaY
                    })
                }
            }
            mouseScrollElement.addEventListener('wheel', mouseScrollEventHandler, true);
            this.mouseScrollEventListener = mouseScrollEventHandler;
        },
        //移除鼠标滚动事件
        removeMouseScrollHandler() {
            const mouseScrollElement = document.querySelector('.screen');
            if (this.mouseScrollEventListener) {
                mouseScrollElement.removeEventListener('wheel', this.mouseScrollEventListener, true)
                this.mouseScrollEventListener = null;
            }
        },
        //改变标签栏
        changeCheckbox() {
            //鼠标键盘同时有
            if (this.checkboxGroup.includes("鼠标") == true && this.checkboxGroup.includes("键盘") == true) {
                if (this.keyFlag == false) {
                    this.addKeyWordDownHandler();
                    this.addKeyWordUpHandler();
                    this.keyFlag = true;
                }
                if (this.miceFlag == false) {
                    this.addClickDownHandler();
                    this.addClickUpHandler();
                    this.addMouseMoveHandler();
                    this.addMouseScrollHander();
                    this.miceFlag = true;
                }
            }
            else if (this.checkboxGroup.includes("鼠标") == true && this.checkboxGroup.includes("键盘") == false) {

                // 没有键盘
                if (this.keyFlag == true) {
                    this.removeKeyWordDownHandler();
                    this.removeKeyWordUpHandler();
                    // this.removeMouseMoveHandler();
                    this.keyFlag = false;
                }
                if (this.miceFlag == false) {
                    this.addClickDownHandler();
                    this.addClickUpHandler();
                    this.addMouseMoveHandler();
                    this.addMouseScrollHander();
                    this.miceFlag = true;
                }
            }
            else if (this.checkboxGroup.includes("键盘") == true && this.checkboxGroup.includes("鼠标") == false) {
                // 只有键盘
                if (this.miceFlag == true) {
                    this.removeClickDownHandler();
                    this.removeMouseMoveHandler();
                    this.removeClickUpHandler();
                    this.removeMouseScrollHandler();
                    this.miceFlag = false;
                }
                if (this.keyFlag == false) {
                    this.addKeyWordDownHandler();
                    // this.addMouseMoveHandler();
                    this.addKeyWordUpHandler();
                    this.keyFlag = true;
                }
            }
            else {
                if (this.keyFlag == true) {
                    this.removeKeyWordDownHandler();
                    this.removeKeyWordUpHandler();
                    this.keyFlag = false;
                }
                if (this.miceFlag == true) {
                    this.removeClickDownHandler();
                    this.removeClickUpHandler();
                    this.removeMouseScrollHandler();
                    this.removeMouseMoveHandler();
                    this.miceFlag = false;
                }
            }
        },

        //处理窗口大小变化的函数
        handleWindowResize() {
            // console.log(window.innerWidth, window.innerHeight);
            this.currentWindowSize.width = window.innerWidth
            this.currentWindowSize.height = window.innerHeight - 60 //减去上边栏，防止出现滚动条
            console.log("窗口变化1", this.currentWindowSize.width, this.currentWindowSize.height);
        },
        //结束连接的请求
        async endTime(name) {
            await post("/endEquipmentCtrl", name);
        },
        //统一进行结束的函数
        async commonHandleFinish() {
            this.endTime({ bectrlEquipName: this.baseInfo.name })
            clearInterval(this.timer)
            this.timer = null;
            //清除本地设备缓存
            localStorage.removeItem("startEquipment");
            // this.$router.replace({ path: '/dashboard' });
            window.opener = null;
            window.open('', '_self');
            window.close();
        },
        //结束控制
        async finishControl() {

            if (confirm("确定结束控制吗？")) {
                this.commonHandleFinish();
            }
        },
        //在关闭页面的时候触发
        async beforeunloadHandler() {
            //这里不能写两个请求，要不然第二个请求不会发送
            await post("/endEquipmentCtrl", { bectrlEquipName: this.baseInfo.name })
            this.endTime({ bectrlEquipName: this.baseInfo.name })
        },
    },
    created() {


        //获取窗口大小以及触发窗口变化的函数
        //在挂载状态之前，获取当前窗口的大小,初次赋值窗口大小
        this.handleWindowResize();
        //当窗口发生变化的时候，调用窗口大小的函数
        window.addEventListener('resize', this.handleWindowResize);
    },
    async mounted() {
        //禁止浏览器后退
        history.pushState(null, null, document.URL);
        window.addEventListener("popstate", function () {
            history.pushState(null, null, document.URL);
        });


        //禁用本地的快捷键
        window.onkeydown = window.onkeyup = window.onkeypress = function (event) {
            // 判断是否按下F12，F12键码为123
            if (event.keyCode === 123) {
                event.preventDefault(); // 阻止默认事件行为
                window.event.returnValue = false;
            }
        };
        document.onkeydown = function (event) {
            var ev = window.event || event;
            var code = ev.keyCode || ev.which;
            if (event.ctrlKey && window.event.keyCode == 65) {   //禁用ctrl + a 功能
                return false;
            }

            if (event.ctrlKey && window.event.keyCode == 67) {	//禁用ctrl + c 功能
                return false;
            }

            if (event.ctrlKey && window.event.keyCode == 83) {	//禁用ctrl + s 功能
                return false;
            }

            if (event.ctrlKey && window.event.keyCode == 86) {	//禁用ctrl + v 功能
                return false;
            }
            if (code == 116) {
                ev.keyCode ? ev.keyCode = 0 : ev.which = 0;
                cancelBubble = true;
                return false;
            }

        }

        //设置鼠标不能右键以及右键刷新
        document.oncontextmenu = new Function("return false");

        this.handleWindowResize();


        //图片显示逻辑
        let query = this.$route.query;
        // console.log(query);//{name: 'sb1'}
        let getInfo = JSON.parse(localStorage.getItem("startEquipment"))
        //console.log(getInfo);//{CtrlTime: 10, eStatus: 1, imageDir: 'user1-sb1',width:1920,height:1080}
        // this.deadLine = Date.now() + 1000 * 60 * getInfo.CtrlTime;

        this.deadLine = Date.now() + 1000 * 60 * getInfo.CtrlTime;
        let currentParam = { width: getInfo.width, height: getInfo.height };
        this.imgParam = { ...currentParam };

        // console.log("当前图片的大小情况", this.imgParam);
        // this.handleImgResize();
        //记录下来
        this.baseInfo = { ...getInfo, ...query }


        // console.log(this.baseInfo);
        /*
        CtrlTime: "10"
        eStatus: 1
        imageDir: "user1-sb1"
        name: "sb1"
        */


        // this.imgUrl = `${process.env.VUE_APP_URL1}/image/${this.baseInfo.imageDir}.jpeg?timer=${Date.now()}`
        // console.log(this.imgUrl);
        this.timer = setInterval(() => {
            try {
                this.imgUrl = `${process.env.VUE_APP_URL1}/image/${this.baseInfo.imageDir}.jpeg?timer=${Date.now()}`
                // console.log(this.imgUrl);
            } catch (error) {
                this.imgUrl = `${process.env.VUE_APP_URL1}/image/${this.baseInfo.imageDir}.jpeg?timer=${Date.now()}`
                console.log("出错了");
                clearInterval(this.timer);
                this.timer = null;
            }
        }, 300)
        this.isShow = true;


    },
    beforeDestroy() {
        //在函数结束之前，把当前事件清除掉
        window.removeEventListener('resize', this.handleWindowResize);
    },
    async destroyed() {
        this.timer = null
        this.endTime({ bectrlEquipName: this.baseInfo.name })
        clearInterval(this.timer)
        //加载
        window.removeEventListener('beforeunload', e => this.beforeunloadHandler(e))
    },


}
</script>
<style scoped>
.container {
    margin: 0 auto;
    width: 100vw;
    overflow-y: hidden;
    height: 100vh;
    overflow-x: hidden;
}

.app-header {
    background-color: rgb(72, 72, 72);
    color: black;
    text-align: center;
    color: #fff;
    display: flex;
    align-items: center;
    font-size: 1.2rem;
    padding: 0;
    height: 8vh;
    min-width: 50rem;
    /* min-height: 4rem; */
    /* max-height: 4rem; */
}

.app-header .left {
    display: flex;
    flex: 1;
    align-items: center;
}

.app-header .center {
    flex: 1;
}

.app-header .right {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: flex-end;
    text-align: right;
    margin-right: 2rem;
}


/* 深度选择器 */
.el-statistic>>>.title {
    color: white;
}

/* 屏幕不让拖动 */
.screen {
    -webkit-user-drag: none;
}

/* 设置图片的宽度和高度在屏幕的比例 */
.imgStyle {
    width: 96vw;
    height: 92vh;
    /* min-width: 100rem; */
    /* min-height: 50rem; */
}

.right div {
    float: left;
}

.el-statistic>>>.head .title {
    font-size: 1rem;
}

.ShowArea {
    display: flex;
}

@media only screen and (max-width: 1600px) and (min-width: 1280px) {

    html,
    body {
        font-size: 14px;
    }
}

@media only screen and (max-width: 1280px) and (min-width: 960px) {
    html {
        font-size: 12px;
    }
}

@media only screen and (max-width: 960px) {
    html {
        font-size: 10px;
    }
}
</style>