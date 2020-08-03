import { Component, OnInit } from '@angular/core';
import { DashboardService } from '../dashboard.service';

/** Interface for keeping track of whether a data set is available for a 
 *  specific date. 
 */
export interface AvailabilityStatus {
  date: string;
  isAvailable: boolean;
}

/** Interface for keeping track of all the availability of a specific source. */
export interface HologramDataAvailability {
  sourceType: string;
  availabilityStatusList: AvailabilityStatus[];
}

/** Component for using the retrieved JSON from server and rendering it
 *  on the dashboard.
 */
@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css'],
  providers: [ DashboardService ]
})
export class DashboardComponent implements OnInit {
  dashboard: HologramDataAvailability[];
  showChipper: boolean;
  showChipperGDPR: boolean;

  constructor(private dashboardService: DashboardService) {
    this.showChipper = false;
    this.showChipperGDPR = false;
    this.dashboard = [];
  }

  ngOnInit(): void {
  }

  renderTemplate(inputData: JSON) {
    for (const key of Object.keys(inputData)) {
      const hologramDataAvailability = {} as HologramDataAvailability;
      hologramDataAvailability.sourceType = key;
      // Need to initialize the list before pushing into it.
      hologramDataAvailability.availabilityStatusList = [];
      inputData[key].forEach((data: [string, boolean]) => {
        let dateInfo = {} as AvailabilityStatus;
        // data is a list of format [date, availability].
        dateInfo.date = data[0];
        dateInfo.isAvailable = data[1];
        hologramDataAvailability.availabilityStatusList.push(dateInfo);
      });
      this.dashboard.push(hologramDataAvailability);
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
      this.dashboardService.getDashboard(system).toPromise()
          .then((data) => this.renderTemplate(data));
    }
  }
}
