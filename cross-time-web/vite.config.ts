/* eslint-disable @typescript-eslint/no-unused-vars */
import { defineConfig, loadEnv } from "vite";
import vue from "@vitejs/plugin-vue";
import path from "path";
import AutoImport from "unplugin-auto-import/vite";
import Components from "unplugin-vue-components/vite";
import { ElementPlusResolver } from "unplugin-vue-components/resolvers";
import { VantResolver } from "@vant/auto-import-resolver";
export default defineConfig(({ command, mode }) => {
  //获取各种环境下对应的变量,传递两个参数，第一个参数是环境，第二个参数是环境的路径
  const env = loadEnv(mode, process.cwd());
  console.log("当前的env", env);
  const serverConfig =
    mode === "development"
      ? {
          proxy: {
            "/lsw": {
              // target: "http://43.139.168.168/api",
              target: "http://118.190.96.96:9090/api",
              changeOrigin: true,
              rewrite: (path: any) => path.replace(/^\/lsw/, ""),
            },
          },
        }
      : {};
  return {
    plugins: [
      vue(),
      AutoImport({
        resolvers: [ElementPlusResolver(), VantResolver()],
      }),
      Components({
        resolvers: [ElementPlusResolver(), VantResolver()],
      }),
    ],
    resolve: {
      alias: {
        "@": path.resolve("./src"), // 相对路径别名配置，使用 @ 代替 src
        "@assets": path.join(__dirname, "src/assets"),
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
    server: serverConfig,
    esbuild: {
      drop: ["console", "debugger"],
    },
    build: {
      rollupOptions: {
        output: {
          manualChunks: (id) => {
            if (id.includes("node_modules")) {
              return "vendor";
            }
          },
        },
      },
    },
  };
});
