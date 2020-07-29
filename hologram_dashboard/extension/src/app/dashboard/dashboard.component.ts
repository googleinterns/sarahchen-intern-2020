import { Component, OnInit } from '@angular/core';
import { RequestHandler } from '../request-handler.component'

// Interface for keeping track of whether a data set is available for a specific
// date.
export interface DateAvailability {
  date: string;
  isAvailable: boolean;
}

// Interface for keeping track of all the availability of a specific source.
export interface HologramDataAvailability {
  sourceType: string;
  dateAvailabilityList: Array<DateAvailability>
}

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css'],
  providers: [ RequestHandler ]
})
export class DashboardComponent implements OnInit {
  dashboard: Array<HologramDataAvailability>;
  showChipper: boolean
  showChipperGDPR: boolean

  constructor(private requestHandler: RequestHandler) { 
    this.showChipper = false;
    this.showChipperGDPR = false;
    this.dashboard = []
  }

  ngOnInit(): void {
  }

  renderTemplate(hologram_data_availability: JSON) {
    for (let key in hologram_data_availability) {
      let data_set_availability = {} as HologramDataAvailability;
      data_set_availability.sourceType = key;
      // Need to initialize the list before pushing into it;
      data_set_availability.dateAvailabilityList = [];
      hologram_data_availability[key].forEach(data => {
        let date_info = {} as DateAvailability;
        // data is a list of format [date, availability]
        date_info.date = data[0];
        date_info.isAvailable = data[1];
        data_set_availability.dateAvailabilityList.push(date_info);
      });
      this.dashboard.push(data_set_availability);
    }
  }

  onClick(system: string) {
    // By setting chipper and chipperGDPR to be complements of themselves we 
    // ensure that if the clicked system is already showing we hide the 
    // dashboard while maintaining ability to switch between dashboards 
    // seamlessly.
    if (system === "Chipper") {
      this.showChipper = !this.showChipper;
      this.showChipperGDPR = false;
    } else if (system === "Chipper_GDPR") {
      this.showChipperGDPR = !this.showChipperGDPR
      this.showChipper = false;
    }
    this.dashboard = [];
    // Only display the dashboard if one of the two are true (they'll never
    // both be true).
    if(this.showChipper || this.showChipperGDPR) {
      this.requestHandler.getDashboard(system).toPromise()
          .then((data) => this.renderTemplate(data));
    }
  }
}
