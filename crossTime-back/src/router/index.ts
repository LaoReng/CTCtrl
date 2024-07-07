import { createRouter, createWebHashHistory } from "vue-router";
import { routes } from "./routes";
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

// router.beforeEach((to:any) => {
//   window.document.title = 'CTCtrl-' + to.meta.title
// })

export default router;
