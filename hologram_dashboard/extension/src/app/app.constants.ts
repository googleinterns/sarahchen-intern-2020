export class AppSettings {
  public static API_ENDPOINT='http://localhost:8000/';
}

// Interface for keeping track of whether a data set is available for a specific
// date.
export interface DateAvailability {
  date: string;
  isAvailable: boolean;
}

// Interface for keeping track of all the availability of a specific source.
export interface HologramDataAvailability {
  sourceType: string;
  dateAvailabilityList: Array<DateAvailability>;
}