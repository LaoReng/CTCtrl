import { createApp } from "vue";
//svg的配置代码
import "virtual:svg-icons-register";
//引入仓库
import pinia from "./store";
//引入路由
import router from "./router";
import ElementPlus from "element-plus";
import App from "@/App.vue";
import "element-plus/dist/index.css";
//暗黑模式
// import "element-plus/theme-chalk/dark/css-vars.css";
import "@/styles/reset.scss";
//@ts-expect-error忽略当前文件ts类型的检测否则有红色提示(打包会失败)
import zhCn from "element-plus/dist/locale/zh-cn.mjs";
import * as ElementPlusIconsVue from '@element-plus/icons-vue'

//引入路由鉴权
import "./permisson.ts";
// 登录背景
import Particles from "vue3-particles";
const app = createApp(App)
for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component)
}

app.use(router);

//把我们自定义的组件注册为全局组件
app.use(ElementPlus, {
  locale: zhCn,
});
app.use(router);
app.use(pinia);
app.use(Particles);
//一定要先注册，再加载
app.mount("#app");
