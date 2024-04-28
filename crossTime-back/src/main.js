import Vue from "vue";
import App from "./App.vue";
import router from "./router";
import "./plugins/element";
// 引入nprogress插件
import "./nprogress.css";
// 引入http插件
import "./plugins/http";
//lsw新增(导入vant)
import Vant from 'vant';
import 'vant/lib/index.css';
Vue.config.productionTip = false;
Vue.use(Vant);
new Vue({
  router,
  render: (h) => h(App),
}).$mount("#app");
