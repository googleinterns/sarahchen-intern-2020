import { Component, OnInit } from '@angular/core';
import { RequestHandler } from '../request-handler.component'

export interface DateAvailability {
  date: string;
  isAvailable: boolean;
}

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
  chipper: boolean
  chipperGDPR: boolean
  display: boolean

  constructor(private requestHandler: RequestHandler) { 
    this.chipper = false;
    this.chipperGDPR = false;
    this.display = false;
    this.dashboard = []
  }

  ngOnInit(): void {
  }

  renderTemplate(hologram_data_availability: JSON, system: string) {
    for (let key in hologram_data_availability) {
      let data_set_availability = {} as HologramDataAvailability;
      data_set_availability.sourceType = key;
      data_set_availability.dateAvailabilityList = [];
      hologram_data_availability[key].forEach(data => {
        let date_info = {} as DateAvailability;
        date_info.date = data[0];
        date_info.isAvailable = data[1];
        data_set_availability.dateAvailabilityList.push(date_info);
      });
      this.dashboard.push(data_set_availability);
    }
  }

  onClick(system: string) {
    if (system === "Chipper") {
      if (this.chipper) {
        this.chipper = false;
      } else {
        this.chipper = true;
        this.chipperGDPR = false;
      }
    } else if (system === "Chipper_GDPR") {
      if (this.chipperGDPR) {
        this.chipperGDPR = false;
      } else {
        this.chipperGDPR = true;
        this.chipper = false;
      }
    }
    this.dashboard = [];
    this.display = (this.chipper || this.chipperGDPR);
    if(this.display) {
      this.requestHandler.getDashboard(system).toPromise()
          .then((data) => this.renderTemplate(data, system));
    }
  }
}
