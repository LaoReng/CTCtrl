//注册全局自定义插件
import SVGIcon from "@/components/SVGIcon/index.vue";
// 如果您正在使用CDN引入，请删除下面一行。
import * as ElementPlusIconsVue from "@element-plus/icons-vue";
const allComponent: any = { SVGIcon };
export default {
  install(app: any) {
    // console.log(app);
    // console.log(allComponent);//{SVGIcon: {…}, Pagination: {…}}
    //遍历对象，来进行循环导入
    Object.keys(allComponent).forEach((key) => {
      //通过这里来注册全局自定义组件
      app.component(key, allComponent[key]);
    });
    for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
      app.component(key, component);
    }
  },
};
