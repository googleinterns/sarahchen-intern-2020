import { Component, OnInit } from '@angular/core';
import { RequestHandler } from '../request-handler.component'

export interface DateAvailability {
  date: string;
  status: boolean;
}

export interface HologramDataAvailability {
  name: string;
  dates: Array<DateAvailability>
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
  chipper_gdpr: boolean
  display: boolean

  constructor(private requestHandler: RequestHandler) { 
    this.chipper = false;
    this.chipper_gdpr = false;
    this.display = false;
    this.dashboard = []
  }

  ngOnInit(): void {
  }

  renderTemplate(hologram_data_availability: JSON, system: string) {
    for (let key in hologram_data_availability) {
      let data_set_availability = {} as HologramDataAvailability;
      data_set_availability.name = key;
      data_set_availability.dates = [];
      hologram_data_availability[key].forEach(data => {
        let date_info = {} as DateAvailability;
        date_info.date = data[0];
        date_info.status = data[1];
        data_set_availability.dates.push(date_info);
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
        this.chipper_gdpr = false;
      }
    } else if (system === "Chipper_GDPR") {
      if (this.chipper_gdpr) {
        this.chipper_gdpr = false;
      } else {
        this.chipper_gdpr = true;
        this.chipper = false;
      }
    }
    this.dashboard = [];
    this.display = (this.chipper || this.chipper_gdpr);
    if(this.display) {
      this.requestHandler.getDashboard(system).toPromise()
          .then((data) => this.renderTemplate(data, system));
    }
  }
}
