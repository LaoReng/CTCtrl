// vite.config.ts
import { defineConfig, loadEnv } from "vite";
import vue from "@vitejs/plugin-vue";
import path from "path";
import AutoImport from "unplugin-auto-import/vite";
import { viteMockServe } from "vite-plugin-mock";
import Components from "unplugin-vue-components/vite";
//引入svg需要用到的插件
import { createSvgIconsPlugin } from "vite-plugin-svg-icons";
import { ElementPlusResolver } from "unplugin-vue-components/resolvers";
export default defineConfig(({ command, mode }) => {
  //获取各种环境下对应的变量,传递两个参数，第一个参数是环境，第二个参数是环境的路径
  const env = loadEnv(mode, process.cwd());
  return {
    plugins: [
      vue(),
      //模拟mock
      viteMockServe({
        enable: command == "serve",
      }),
      AutoImport({
        resolvers: [ElementPlusResolver()],
      }),
      Components({
        resolvers: [ElementPlusResolver()],
      }),
      //svg
      createSvgIconsPlugin({
        //这里代表的是以后的图标会下载到assets文件夹下的icons里
        iconDirs: [path.resolve(process.cwd(), "src/assets/icons")],
        symbolId: "icon-[dir]-[name]",
      }),
    ],
    resolve: {
      alias: {
        "@": path.resolve("./src"), // 相对路径别名配置，使用 @ 代替 src
      },
    },
    //sass全局变量的配置
    css: {
      preprocessorOptions: {
        scss: {
          javascriptEnabled: true,
          additionalData: '@import "./src/styles/variable.scss";',
        },
      },
    },
    //代理跨域
    server: { 
      proxy: {
        '/wmq': {
          target: 'http://43.139.168.168/api/back',//目标服务器地址
          changeOrigin: true,
          rewrite: (path) => path.replace(/^\/wmq/, '')
        },
      }
    }
  }
});
