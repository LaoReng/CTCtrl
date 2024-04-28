import Vue from "vue";
import App from "./App.vue";
import router from "./router";
import "./plugins/element";
import Vant from 'vant';
import Direction from 'vue-direction-key'

// 引入nprogress插件
import "./nprogress.css";
import 'vant/lib/index.css';
// 引入http插件
import "./plugins/request";
Vue.config.productionTip = false;
Vue.use(Vant);
Vue.use(Direction)
new Vue({
  router,
  render: (h) => h(App),
}).$mount("#app");
