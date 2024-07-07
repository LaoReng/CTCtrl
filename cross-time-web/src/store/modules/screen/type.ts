import { dataSummaryType, weatherBody } from "@/api/screen/type";
// interface listType {
//   name: string;
//   type: any;
//   count: number;
// }
export interface screenStoreType {
  dataSummaryInfo: dataSummaryType;
  online: number;
  isShowW: boolean;
  isShowW1: boolean;
  isShowW2: boolean;
  // vnodeFor: listType[];
  allEquipmentList: any[];
  points: any[];
  ctrlTobeCtrlList: any[];
  weatherData: weatherBody;
  isShowScreen: boolean;
}
