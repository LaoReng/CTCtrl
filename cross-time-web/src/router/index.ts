import { createRouter, createWebHashHistory } from "vue-router";
import { routes } from "./routes";
// import { defineRoutes } from "@/permisson";
const router = createRouter({
  history: createWebHashHistory(),
  routes,
  scrollBehavior() {
    return {
      left: 0,
      top: 0,
    };
  },
});
export default router;
