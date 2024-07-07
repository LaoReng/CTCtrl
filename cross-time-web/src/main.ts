import { createApp } from "vue";
//引入粒子特效
import Particles from "vue3-particles";
//引入仓库
import pinia from "./store";
//引入路由
import router from "./router";
//引入elementplus
import ElementPlus from "element-plus";
import App from "@/App.vue";
//引入element的样式
import "element-plus/dist/index.css";
//暗黑模式
import "element-plus/theme-chalk/dark/css-vars.css";
//引入全局样式重置文件
import "@/styles/reset.scss";
//@ts-expect-error忽略当前文件ts类型的检测否则有红色提示(打包会失败)
import zhCn from "element-plus/dist/locale/zh-cn.mjs";
//引入全局的自定义组件
import GlobalComponents from "@/components/index";
//引入路由鉴权
import "./permisson.ts";
//引入vant
import "vant/lib/index.css";
import { NoticeBar } from "vant";
//引入MarkDown
// @ts-expect-error忽略当前文件ts类型的检测否则有红色提示(打包会失败)
import VueMarkdownEditor from "@kangc/v-md-editor";
import "@kangc/v-md-editor/lib/style/base-editor.css";
// @ts-expect-error忽略当前文件ts类型的检测否则有红色提示(打包会失败)
import vuepressTheme from "@kangc/v-md-editor/lib/theme/vuepress.js";
import "@kangc/v-md-editor/lib/theme/style/vuepress.css";
// @ts-expect-error忽略当前文件ts类型的检测否则有红色提示(打包会失败)
import Prism from "prismjs";
VueMarkdownEditor.use(vuepressTheme, {
  Prism,
});

const app = createApp(App);
//我们正常是app.component('')来注册自定义全局组件的，但是假如我们随着自定义组件的增多
//就会很臃肿，所以我们新建一个文件用于帮我们进行管理
//一定要先注册，再加载
app.use(GlobalComponents);
app.use(Particles);
//把我们自定义的组件注册为全局组件
app.use(ElementPlus, {
  locale: zhCn,
});
app.use(NoticeBar);
app.use(VueMarkdownEditor);
//引入仓库
app.use(pinia);
//引入路由
app.use(router);

app.mount("#app");
