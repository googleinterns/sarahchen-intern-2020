import { Component, OnInit } from '@angular/core';
import { RequestHandler } from '../request-handler.component';
import { HologramDataAvailability, DateAvailability } from '../app.constants';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css'],
  providers: [ RequestHandler ]
})
export class DashboardComponent implements OnInit {
  dashboard: Array<HologramDataAvailability>;
  showChipper: boolean;
  showChipperGDPR: boolean;

  constructor(private requestHandler: RequestHandler) { 
    this.showChipper = false;
    this.showChipperGDPR = false;
    this.dashboard = [];
  }

  ngOnInit(): void {
  }

  renderTemplate(inputData: JSON) {
    for (let key of Object.keys(inputData)) {
      let hologramDataAvailability = {} as HologramDataAvailability;
      hologramDataAvailability.sourceType = key;
      // Need to initialize the list before pushing into it;
      hologramDataAvailability.dateAvailabilityList = [];
      inputData[key].forEach(data => {
        let dateInfo = {} as DateAvailability;
        // data is a list of format [date, availability]
        dateInfo.date = data[0];
        dateInfo.isAvailable = data[1];
        hologramDataAvailability.dateAvailabilityList.push(dateInfo);
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
      this.requestHandler.getDashboard(system).toPromise()
          .then((data) => this.renderTemplate(data));
    }
  }
}
