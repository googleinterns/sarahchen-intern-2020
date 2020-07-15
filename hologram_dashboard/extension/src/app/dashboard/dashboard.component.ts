import { Component, OnInit } from '@angular/core';
import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, throwError } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';

@Injectable()
export class RequestHandler {
  url: string
  constructor(private http: HttpClient) { 
    this.url = "http://localhost:8000/"
  }

  getDashboard(system: string) {
    let html = this.url.concat(system);
    return this.http.get(html);
  }
}

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {
  chipper_dashboard: string;
  chipper_gdpr_dashboard: string;
  request_handler: RequestHandler;

  constructor() { 
    this.chipper_dashboard = "";
    this.chipper_gdpr_dashboard = "";
  }

  ngOnInit(): void {
  }

  onClick(system: string) {
    this.request_handler.getDashboard(system)
      .subscribe((data: string) => this.chipper_dashboard = data);
    console.log(this.chipper_dashboard);
  }

}
